#include <unistd.h>
#include <csignal>
#include <cstring>
#include <sstream>
#include <stacktrace>

#include "../Application.h"
#include "Application.h"

namespace nbn::core::private_ns {

#if defined(NBN_LLVM_COVERAGE_ENABLE)
extern "C" auto llvmProfileWriteFile() -> int asm("__llvm_profile_write_file");
#endif

namespace {

constexpr int kFatalSignalExitCode = 1;

auto isCrashSignal(int signum) noexcept -> bool {
    switch (signum) {
        case SIGSEGV:
        case SIGBUS:
        case SIGFPE:
        case SIGILL:
        case SIGABRT:
            return true;
        default:
            return false;
    }
}

auto isCatchableSignal(int signum) noexcept -> bool {
    return signum != SIGKILL && signum != SIGSTOP;
}

}  // namespace

auto stackTraceMessage(std::string_view context) noexcept -> std::string {
    std::ostringstream output;
    output << context << '\n' << std::stacktrace::current();
    return output.str();
}

// Termination callbacks

auto signalInterruptHandler(int signum) noexcept -> void {
    static bool isInterrupted = false;
    if (!isInterrupted) {
        isInterrupted = true;
        // Keep signal handling non-throwing: nbn::log::fatal throws and can trigger std::terminate in signal context.
        nbn::log::error(std::format("Signal {} ({}) received. Terminating...", signum, ::strsignal(signum)));
        nbn::core::Application::signalInterrupted()->emit(signum);
    }
}

auto c_terminateHandler() noexcept -> void {
    nbn::log::debug("Terminate called. Attempting to log current exception.");
    auto currentException{std::current_exception()};
    if (currentException != nullptr) {
        try {
            nbn::log::debug("Terminate called. Rethrowing current exception to log it.");
            std::rethrow_exception(currentException);
        } catch (const std::exception& e) {
            auto message{std::format("Terminate called. Current exception: {}", e.what())};
            nbn::log::error(message);
        } catch (...) {
            nbn::log::error("Terminate called. Current exception: unknown");
        }
        nbn::log::error(stackTraceMessage("Stack trace at terminate:"));
    }

    nbn::core::Application::quit(1);
}

// std::vector<std::string> signalsAsStringVector()  =

// Hardware-fault signals (SIGSEGV, SIGBUS, SIGFPE, SIGILL, SIGABRT): log the error
// Note: if the crash occurred while the logger mutex was held, this will deadlock.
auto hardwaretSignalHandler(int signum) noexcept -> void {
    nbn::log::debug(std::format("Signal {} ({}) received. Entering hardware-fault handler.", signum, ::strsignal(signum)));
    nbn::log::error(
        stackTraceMessage(std::format("Fatal: signal {} ({}) received - exiting with code 1", signum, ::strsignal(signum))));
    nbn::core::Application::signalAboutToQuit()->emit();
// Do not enter the normal application shutdown path from a signal handler. The
// handler runs on the faulting thread, and stopping/joining the thread manager
// from there can deadlock. The logger has already accepted the diagnostic above;
// terminate without invoking destructors or joining threads.
#if defined(NBN_LLVM_COVERAGE_ENABLE)
    (void)llvmProfileWriteFile();
#endif
    _exit(kFatalSignalExitCode);
}

Application::Application()
    : m_notInitializedProgramName("Application is not initialized"), m_programName(m_notInitializedProgramName) {
    // POSIX signals are not available in embedded PlatformIO/ESP32 targets.
#if !defined(PLATFORMIO_BUILD)
    // Hardware-fault signals use a crash handler that logs the error and exits with code 1.
    // All other signals use the normal interrupt handler.
    constexpr int SIGNALS_COUNT = 32;
    for (int i = 1; i < SIGNALS_COUNT; ++i) {
        // SIGKILL and SIGSTOP cannot be caught or overridden by POSIX. Avoid
        // asking the runtime to install handlers for them; Valgrind reports
        // those attempts as ignored sigaction requests.
        // SIGCHLD is generated whenever a child process exits. It is not an
        // application interruption signal, and routing it through the normal
        // logging/dispatch path would call non-async-signal-safe code while
        // another thread may be inside libc.
        if (!isCatchableSignal(i) || i == SIGCHLD) {
            continue;
        }
        std::signal(i, isCrashSignal(i) ? hardwaretSignalHandler : signalInterruptHandler);
    }
#endif
    std::set_terminate(c_terminateHandler);
    setTerminateHandler([this]() noexcept { quit(1); });
}

Application::~Application() {
    if (m_spLogger != nullptr) {
        m_spLogger->stop();
    }
}

Application& Application::self() {
    static Application self;
    return self;
}

void Application::init(std::vector<std::string> args) {
    if (m_spLogger == nullptr) {
        m_spLogger = std::make_shared<Logger>();
    }
    std::filesystem::path programPath(args[0]);
    m_programName = programPath.filename().string();
    m_programPath = programPath.parent_path();
    m_arguments = std::move(args);
    m_arguments.erase(m_arguments.begin());
}

auto Application::submit(std::shared_ptr<Task> spTask) -> void {
    if (m_spThreadsManager == nullptr) {
        m_spThreadsManager = std::make_unique<ApplicationThreadsManager>();
    }
    m_spThreadsManager->submit(std::move(spTask));
}

auto Application::submit(std::shared_ptr<Thread> spTask) -> void {
    if (m_spThreadsManager == nullptr) {
        m_spThreadsManager = std::make_unique<ApplicationThreadsManager>();
    }
    m_spThreadsManager->submit(std::move(spTask));
}

[[nodiscard]] auto Application::exec() -> int {
    if (m_spThreadsManager == nullptr) {
        m_spThreadsManager = std::make_unique<ApplicationThreadsManager>();
    }

    try {
        m_spThreadsManager->run();
        m_spThreadsManager->wait();
    } catch (const std::exception& ex) {
        m_quitCode = 1;
        try {
            nbn::log::error(stackTraceMessage(std::format("Uncaught exception: {}", ex.what())));
            throw;
        } catch (...) {
            nbn::log::debug(std::format("Application quit with code {}", m_quitCode));
            Application::signalUncaughtAsyncException()->emit();
            return m_quitCode;
        }
    } catch (...) {
        m_quitCode = 2;
        try {
            // Log the error
            nbn::log::fatal("Unknown exception occurred");
        } catch (...) {
            m_quitCode = 2;
        }
    }

    signalAboutToQuit()->emit();

    return m_quitCode;
}

void Application::quit(int exitCode) {
    m_quitCode = exitCode;
    // Stopping the threads manager wakes the threadStateChecker condition variable
    // so ApplicationThreadsManager::Impl::wait() can unblock and exec() can return.
    if (m_spThreadsManager != nullptr) {
        m_spThreadsManager->stop();
    }
}

auto Application::setTerminateHandler(std::function<void()> handler) -> void {
    m_terminateHandler = std::move(handler);
}

auto Application::getTerminateHandler() -> std::function<void()> {
    return m_terminateHandler;
}

auto Application::factoryRegister(std::string_view name, std::function<std::shared_ptr<Object>()> creator) -> bool {
    m_creators[std::string(name)] = creator;
    return true;
}

auto Application::factoryCreate(std::string_view name) -> std::shared_ptr<Object> {
    auto it{m_creators.find(std::string(name))};
    ::nbn::log::fatal_if(it == m_creators.end(), std::format("Class {} is not registered", name));
    return it->second();
}

auto Application::factoryRegisteredClasses() -> std::vector<std::string> {
    std::vector<std::string> classes;
    for (const auto& [name, _] : m_creators) {
        classes.emplace_back(name);
    }
    return classes;
}

auto Application::factoryHas(std::string_view name) -> bool {
    return m_creators.find(std::string(name)) != m_creators.end();
}

}  // namespace nbn::core::private_ns