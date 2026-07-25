#include "Watcher.h"

#include "../FiniteStateMachine.h"

#include <chrono>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

namespace nbn::core::filesystem {

class Watcher::Impl final : public fsm::Machine {
    struct FileState {
        std::filesystem::file_time_type lastWriteTime{};
        std::uintmax_t size{};

        [[nodiscard]] auto operator==(const FileState& other) const -> bool {
            return lastWriteTime == other.lastWriteTime && size == other.size;
        }
    };

    using file_states_t = std::map<std::filesystem::path, FileState>;

    Watcher* m_pDecl;
    std::filesystem::path m_directory;
    file_states_t m_fileStates{};
    std::vector<std::filesystem::path> m_pendingChanges{};
    static constexpr auto kPollInterval = std::chrono::milliseconds{25};

   public:
    Impl(Watcher* pDecl, std::filesystem::path directory) : m_pDecl{pDecl}, m_directory{std::move(directory)} {
        const auto watchingState = addState("Watching");
        addTransition(
            watchingState, watchingState, [this]() { return hasChanges(); },
            [this]() {
                emitChanges();
                return true;
            });
    }

    [[nodiscard]] auto directory() const -> const std::filesystem::path& { return m_directory; }

    auto prepare() -> void { m_fileStates = readFileStates(); }

   private:
    auto readFileStates() -> file_states_t {
        file_states_t currentStates{};
        try {
            for (const auto& entry : std::filesystem::directory_iterator{m_directory}) {
                if (!entry.is_regular_file()) {
                    continue;
                }
                currentStates.emplace(entry.path(), FileState{entry.last_write_time(), entry.file_size()});
            }
        } catch (const std::filesystem::filesystem_error&) {
            currentStates.clear();
        }
        return currentStates;
    }

    [[nodiscard]] auto hasChanges() -> bool {
        std::this_thread::sleep_for(kPollInterval);

        const auto currentStates = readFileStates();

        m_pendingChanges.clear();
        for (const auto& [path, state] : currentStates) {
            const auto previous = m_fileStates.find(path);
            if (previous == m_fileStates.end() || !(previous->second == state)) {
                m_pendingChanges.push_back(path);
            }
        }
        for (const auto& [path, _] : m_fileStates) {
            if (currentStates.find(path) == currentStates.end()) {
                m_pendingChanges.push_back(path);
            }
        }

        m_fileStates = std::move(currentStates);
        return !m_pendingChanges.empty();
    }

    auto emitChanges() -> void {
        for (const auto& path : m_pendingChanges) {
            m_pDecl->signalChanged()->emit(path.string());
        }
        m_pendingChanges.clear();
    }
};

Watcher::Watcher(std::filesystem::path directory) : m_spImpl{} {
    std::error_code error;
    if (!std::filesystem::is_directory(directory, error)) {
        throw std::invalid_argument{"Watcher requires an existing directory"};
    }
    m_spImpl = std::make_unique<Impl>(this, std::filesystem::absolute(directory, error).lexically_normal());
}

Watcher::~Watcher() {
    stop();
    wait();
}

auto Watcher::directory() const -> const std::filesystem::path& {
    return m_spImpl->directory();
}

void Watcher::run() {
    m_spImpl->prepare();
    m_spImpl->run();
    signalStarted()->emit();
}

void Watcher::stop() {
    m_spImpl->stop();
}

[[nodiscard]] auto Watcher::isRunning() const -> bool {
    return m_spImpl->isRunning();
}

[[nodiscard]] auto Watcher::isStopRequested() const -> bool {
    return m_spImpl->isStopRequested();
}

void Watcher::wait() {
    m_spImpl->wait();
    signalStopped()->emit();
}

}  // namespace nbn::core::filesystem
