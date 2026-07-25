#include <csignal>
#include <cstdlib>

#include <nbn/core/Application.h>

// The crash test is a standalone process. Starting it through the generated
// unit-test runner would initialize Application first and start Logger's
// worker std::jthread before the crash path is exercised.
auto main() -> int {
    // Construct Application and install its signal handler while Logger has
    // not been started and owns no worker thread.
    (void)nbn::core::Application::signalAboutToQuit();
    ::raise(SIGSEGV);
    return EXIT_FAILURE;
}
