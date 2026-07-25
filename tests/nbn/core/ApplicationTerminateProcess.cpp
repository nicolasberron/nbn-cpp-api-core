#include <nbn/core/Application.h>

#include <sys/wait.h>
#include <unistd.h>
#include <array>
#include <csignal>
#include <cstdlib>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

constexpr int kChildArgumentCount = 2;
constexpr int kFailureExitCode = 1;
constexpr int kSuccessExitCode = 0;
constexpr int kUnknownExceptionValue = 1;

struct TerminateChildArguments {
    const char* executable;
    const char* exceptionType;
};

auto runTerminateChild(TerminateChildArguments arguments) -> bool {
    const auto childPid = fork();
    if (childPid < 0) {
        return false;
    }
    if (childPid == 0) {
        std::string executableArgument{arguments.executable};
        std::string exceptionArgument{arguments.exceptionType};
        std::array<char*, 3> childArguments{executableArgument.data(), exceptionArgument.data(), nullptr};
        (void)execv(arguments.executable, childArguments.data());
        _exit(kFailureExitCode);
    }

    int childStatus{0};
    if (waitpid(childPid, &childStatus, 0) < 0) {
        return false;
    }
    return WIFEXITED(childStatus) && WEXITSTATUS(childStatus) == kFailureExitCode;
}

}  // namespace

[[noreturn]] auto terminateWithException(std::string_view exceptionType) noexcept -> void {
    try {
        if (exceptionType == "unknown") {
            throw kUnknownExceptionValue;
        }
        throw std::runtime_error{"terminate coverage exception"};
    } catch (...) {
        std::terminate();
    }
}

auto main(int argc, char* argv[]) noexcept -> int {
    const std::span<char*> arguments{argv, static_cast<std::size_t>(argc)};
    if (argc == kChildArgumentCount) {
        std::vector<std::string> applicationArguments;
        applicationArguments.reserve(arguments.size());
        for (const char* argument : arguments) {
            applicationArguments.emplace_back(argument);
        }
        nbn::core::Application::init(std::move(applicationArguments));
        terminateWithException(arguments[1]);
    }

    if (argc != 1 || !runTerminateChild({arguments[0], "standard"}) || !runTerminateChild({arguments[0], "unknown"})) {
        return kFailureExitCode;
    }
    return kSuccessExitCode;
}