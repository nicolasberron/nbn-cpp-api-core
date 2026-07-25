#include <nbn/core/UnitTests.h>
#include <nbn/core/filesystem/Watcher.h>

#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace nbn::core;

namespace {

constexpr auto kWaitTimeout = std::chrono::seconds{2};

auto waitForChanges(std::condition_variable& condition,
                    std::mutex& mutex,
                    const std::vector<std::string>& changes,
                    size_t expectedCount) -> bool {
    std::unique_lock lock{mutex};
    return condition.wait_for(lock, kWaitTimeout, [&]() { return changes.size() >= expectedCount; });
}

}  // namespace

void test_filesystem_watcher_reports_created_files() {
    const auto directory = std::filesystem::temp_directory_path() / "nbn-filesystem-watcher-test";
    const auto file = std::filesystem::absolute(directory / "created.txt").lexically_normal();
    std::error_code error;
    std::filesystem::remove_all(directory, error);
    std::filesystem::create_directories(directory);

    std::mutex mutex;
    std::condition_variable condition;
    std::string changedPath;
    bool changed{false};
    filesystem::Watcher watcher{directory};
    watcher.signalChanged()->connect([&](const std::string& path) {
        std::lock_guard lock{mutex};
        changedPath = path;
        changed = true;
        condition.notify_one();
    });

    watcher.run();
    {
        std::ofstream output{file};
        output << "created";
    }

    std::unique_lock lock{mutex};
    const auto observed = condition.wait_for(lock, kWaitTimeout, [&]() { return changed; });
    lock.unlock();

    watcher.stop();
    watcher.wait();
    std::filesystem::remove_all(directory, error);

    unit_tests::isTrue("Watcher should report a created file", observed);
    unit_tests::equal("Watcher should report the changed path", file.string(), changedPath);
}

void test_filesystem_watcher_reports_modified_and_removed_files() {
    const auto directory = std::filesystem::temp_directory_path() / "nbn-filesystem-watcher-update-test";
    const auto file = std::filesystem::absolute(directory / "tracked.txt").lexically_normal();
    const auto nestedDirectory = directory / "nested";
    std::error_code error;
    std::filesystem::remove_all(directory, error);
    std::filesystem::create_directories(nestedDirectory);
    {
        std::ofstream output{file};
        output << "initial";
    }

    std::mutex mutex;
    std::condition_variable condition;
    std::vector<std::string> changes;
    filesystem::Watcher watcher{directory};
    watcher.signalChanged()->connect([&](const std::string& path) {
        std::lock_guard lock{mutex};
        changes.push_back(path);
        condition.notify_one();
    });

    unit_tests::equal("Watcher should normalize the watched directory",
                      std::filesystem::absolute(directory).lexically_normal().string(), watcher.directory().string());
    watcher.run();
    unit_tests::isTrue("Watcher should be running", watcher.isRunning());
    unit_tests::isFalse("Watcher should not initially request stop", watcher.isStopRequested());
    std::this_thread::sleep_for(std::chrono::milliseconds{100});

    {
        std::ofstream output{file, std::ios::app};
        output << "updated";
    }
    const auto modified = waitForChanges(condition, mutex, changes, 1);
    std::filesystem::remove(file);
    const auto removed = waitForChanges(condition, mutex, changes, 2);

    watcher.stop();
    unit_tests::isTrue("Watcher should request stop", watcher.isStopRequested());
    watcher.wait();
    unit_tests::isFalse("Watcher should stop running", watcher.isRunning());
    std::filesystem::remove_all(directory, error);

    unit_tests::isTrue("Watcher should report a modified file", modified);
    unit_tests::isTrue("Watcher should report a removed file", removed);
    unit_tests::equal("Watcher should report the modified path", file.string(), changes.front());
    unit_tests::equal("Watcher should report the removed path", file.string(), changes.back());
}

void test_filesystem_watcher_handles_a_removed_directory() {
    const auto directory = std::filesystem::temp_directory_path() / "nbn-filesystem-watcher-removed-directory-test";
    const auto file = directory / "tracked.txt";
    std::error_code error;
    std::filesystem::remove_all(directory, error);
    std::filesystem::create_directories(directory);
    {
        std::ofstream output{file};
        output << "tracked";
    }

    std::mutex mutex;
    std::condition_variable condition;
    bool stopped{false};
    filesystem::Watcher watcher{directory};
    watcher.signalStopped()->connect([&]() {
        std::lock_guard lock{mutex};
        stopped = true;
        condition.notify_one();
    });

    watcher.run();
    std::filesystem::remove_all(directory, error);
    watcher.stop();
    watcher.wait();

    std::unique_lock lock{mutex};
    const auto observed = condition.wait_for(lock, kWaitTimeout, [&]() { return stopped; });
    unit_tests::isTrue("Watcher should emit its stopped signal", observed);
}

void test_filesystem_watcher_stops_before_polling() {
    const auto directory = std::filesystem::temp_directory_path() / "nbn-filesystem-watcher-stop-test";
    std::error_code error;
    std::filesystem::remove_all(directory, error);
    std::filesystem::create_directories(directory);

    filesystem::Watcher watcher{directory};
    watcher.run();
    watcher.stop();
    watcher.wait();

    unit_tests::isFalse("Watcher should not remain running after an immediate stop", watcher.isRunning());
    std::filesystem::remove_all(directory, error);
}

void test_filesystem_watcher_handles_directory_access_errors() {
    constexpr auto kNoPermissions = std::filesystem::perms::none;
    constexpr auto kOwnerDirectoryPermissions =
        std::filesystem::perms::owner_read | std::filesystem::perms::owner_write | std::filesystem::perms::owner_exec;
    const auto directory = std::filesystem::temp_directory_path() / "nbn-filesystem-watcher-permissions-test";
    std::error_code error;
    std::filesystem::remove_all(directory, error);
    std::filesystem::create_directories(directory);

    filesystem::Watcher watcher{directory};
    std::filesystem::permissions(directory, kNoPermissions, std::filesystem::perm_options::replace, error);
    watcher.run();
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    std::filesystem::permissions(directory, kOwnerDirectoryPermissions, std::filesystem::perm_options::replace, error);
    watcher.stop();
    watcher.wait();

    unit_tests::isFalse("Watcher should stop after a directory access error", watcher.isRunning());
    std::filesystem::remove_all(directory, error);
}

void test_filesystem_watcher_recovers_when_directory_disappears() {
    const auto directory = std::filesystem::temp_directory_path() / "nbn-filesystem-watcher-disappearing-test";
    std::error_code error;
    std::filesystem::remove_all(directory, error);
    std::filesystem::create_directories(directory);

    filesystem::Watcher watcher{directory};
    std::filesystem::remove_all(directory, error);
    watcher.run();
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    watcher.stop();
    watcher.wait();

    unit_tests::isFalse("Watcher should recover from a missing directory", watcher.isRunning());
}

void test_filesystem_watcher_ignores_unchanged_files() {
    const auto directory = std::filesystem::temp_directory_path() / "nbn-filesystem-watcher-no-change-test";
    const auto file = directory / "unchanged.txt";
    std::error_code error;
    std::filesystem::remove_all(directory, error);
    std::filesystem::create_directories(directory);
    {
        std::ofstream output{file};
        output << "unchanged";
    }

    filesystem::Watcher watcher{directory};
    watcher.run();
    std::this_thread::sleep_for(std::chrono::milliseconds{150});
    watcher.stop();
    watcher.wait();

    unit_tests::isFalse("Watcher should stop without reporting unchanged files", watcher.isRunning());
    std::filesystem::remove_all(directory, error);
}

void test_filesystem_watcher_rejects_missing_directories() {
    const auto directory = std::filesystem::temp_directory_path() / "nbn-filesystem-watcher-missing";
    std::error_code error;
    std::filesystem::remove_all(directory, error);

    unit_tests::isThrowing<std::invalid_argument>("Watcher should reject a missing directory",
                                                  [&]() { filesystem::Watcher watcher{directory}; });
}
