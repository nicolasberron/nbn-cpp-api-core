#include <chrono>
#include <iostream>

#include "Application.h"
#include "UnitTests.h"
#include "private/Task.h"
#include "private/format_bridge.h"

namespace nbn::core {

class UnitTests::Impl {
   public:
    Impl() = default;
    ~Impl() = default;

    // TODO: source_location
    void addTest(std::string name, std::function<void()> func) { m_tests.emplace_back(name, std::move(func)); }

    void run() {
        for (size_t i = 0; i < m_tests.size(); ++i) {
            m_currentTest = m_tests[i];
            nbn::log::info(std::format(R"(---------- Running test "{}" ----------)", m_currentTest.name));

            auto startTime = std::chrono::high_resolution_clock::now();

            std::string status = "PASSED";

            m_currentTest.func();
            ++m_passedCount;

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            nbn::log::info(std::format(R"({}: Test "{}" took {} ms)", status, m_currentTest.name, duration));
        }

        // The custom CTest integration uses the application exit code as the
        // test result: zero means all tests passed, and the failure count is
        // non-zero when any test failed.
        printResults();
        Application::quit(m_failedCount);
    }

    auto printResults() const -> void {
        nbn::log::info(std::format("Ran {} tests: {} passed, {} failed", m_tests.size(), m_passedCount, m_failedCount));
    }

   private:
    struct Test {
        std::string name;
        std::function<void()> func;
        Test(std::string_view n = "", std::function<void()> f = nullptr) : name{n}, func{std::move(f)} {}
    };

    std::vector<Test> m_tests{};
    Test m_currentTest;
    int m_passedCount{0};
    int m_failedCount{0};
};

// UnitTests implementation

UnitTests::UnitTests() : Task([this]() { m_spImpl->run(); }, Task::RunMode::SingleShot), m_spImpl(std::make_unique<Impl>()) {}

UnitTests::~UnitTests() = default;

void UnitTests::addTest(std::string name, std::function<void()> func) {
    m_spImpl->addTest(std::move(name), std::move(func));
}

}  // namespace nbn::core