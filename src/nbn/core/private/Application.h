#pragma once

#include <atomic>
#include <filesystem>
#include <functional>
#include <iostream>
#include <thread>

#include "../Application.h"
#include "../ConcurrentQueue.h"
#include "../Logger.h"
#include "../interfaces/concurrency.h"
#include "ApplicationThreadsManager.h"
#include "Object.h"
#include "format_bridge.h"

namespace nbn::core::private_ns {

auto stackTraceMessage(std::string_view context) noexcept -> std::string;

class Application : public Object {
   public:
    Application();
    ~Application() override;

    static Application& self();

    void init(std::vector<std::string> args);

    [[nodiscard]] auto isApplicationInitialized() const -> bool { return m_programName != m_notInitializedProgramName; }

    [[nodiscard]] auto programName() const -> const std::string& { return m_programName; }

    [[nodiscard]] auto programPath() const -> const std::filesystem::path& { return m_programPath; }

    [[nodiscard]] auto arguments() const -> const std::vector<std::string>& { return m_arguments; }

    auto submit(std::shared_ptr<Task> spTask) -> void;
    auto submit(std::shared_ptr<Thread> spTask) -> void;

    [[nodiscard]] auto exec() -> int;

    void quit(int exitCode);

    [[nodiscard]] auto logger() -> std::shared_ptr<nbn::core::Logger>& { return m_spLogger; }

    auto signalInitialized() -> signal_ptr<> { return signal<>(); }
    auto signalAboutToQuit() -> signal_ptr<> { return signal<>(); }
    auto signalInterrupted() -> signal_ptr<int> { return signal<int>(); }
    auto signalUncaughtAsyncException() -> signal_ptr<> { return signal<>(); }
    auto signalLogDebug() -> signal_ptr<std::string> { return signal<std::string>(); }
    auto signalLogInfo() -> signal_ptr<std::string> { return signal<std::string>(); }
    auto signalLogWarning() -> signal_ptr<std::string> { return signal<std::string>(); }
    auto signalLogError() -> signal_ptr<std::string> { return signal<std::string>(); }
    auto signalLogFatal() -> signal_ptr<std::string> { return signal<std::string>(); }

    auto setTerminateHandler(std::function<void()> handler) -> void;
    auto getTerminateHandler() -> std::function<void()>;

    // Factory registration / creation
    [[nodiscard]] auto factoryRegister(std::string_view name, std::function<std::shared_ptr<Object>()> creator) -> bool;

    [[nodiscard]] auto factoryCreate(std::string_view name) -> std::shared_ptr<Object>;

    template <typename Class>
    [[nodiscard]] auto factoryCreate(std::string_view name) -> std::shared_ptr<Class> {
        return std::dynamic_pointer_cast<Class>(factoryCreate(name));
    }

    [[nodiscard]] auto factoryRegisteredClasses() -> std::vector<std::string>;

    [[nodiscard]] auto factoryHas(std::string_view name) -> bool;

   private:
    std::function<void()> m_terminateHandler{nullptr};

    std::string m_notInitializedProgramName{};
    std::string m_programName{};
    std::filesystem::path m_programPath{};
    std::vector<std::string> m_arguments{};

    std::unordered_map<std::string, std::function<std::shared_ptr<Object>()>> m_creators{};

    int m_quitCode{0};

    std::unique_ptr<ApplicationThreadsManager> m_spThreadsManager{};

    std::shared_ptr<nbn::core::Logger> m_spLogger{};
};

}  // namespace nbn::core::private_ns
