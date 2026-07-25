#include <nbn/core/ElapsedTimer.h>
#include <nbn/core/FiniteStateMachine.h>
#include <nbn/core/Logger.h>
#include <nbn/core/Timer.h>
#include <nbn/core/UnitTests.h>

#include <stdexcept>

// Test FiniteStateMachine

using namespace nbn::core;

namespace {

constexpr auto kIdleStateName = "Idle";
constexpr auto kRunningStateName = "Running";

}  // namespace

class RedLightCycle : public fsm::Machine {
   public:
    auto name() -> property_ptr<std::string> { return property<std::string>(); }

    RedLightCycle() {
        // Add states
        m_spGreenLightState = fsm::Machine::addState(
            "Green light",
            [this]() -> bool {
                // Stop fsm when coming back to green light
                if (m_testedStates > 0) {
                    nbn::log::info(std::format("Stopping fsm {}", name()->get()));
                    return false;
                }
                m_stateCallOrder += "ge";
                ++m_testedStates;
                nbn::log::info(std::format("Entering greenLightState, tested states: {}", m_testedStates));
                return true;
            },
            [this]() -> bool {
                // When the fsm is started, it is always already in its initial state;
                // so m_testedStates has not been incremented yet if it is the first cycle
                if (m_testedStates == 0) {
                    ++m_testedStates;
                    nbn::log::info(std::format("Entering greenLightState, tested states: {}", m_testedStates));
                } else {
                    nbn::log::info("Leaving greenLightState");
                }
                m_stateCallOrder += "gl";
                return true;
            });

        m_spYellowLightState = addState(
            "Yellow light",
            [this]() -> bool {
                m_stateCallOrder += "ye";
                ++m_testedStates;
                nbn::log::info(std::format("Entering yellowLightState, tested states: {}", m_testedStates));
                return true;
            },
            [this]() -> bool {
                m_stateCallOrder += "yl";
                nbn::log::info("Leaving yellowLightState");
                return true;
            });

        m_spRedLightState = addState(
            "Red light",
            [this]() -> bool {
                m_stateCallOrder += "re";
                ++m_testedStates;
                nbn::log::info(std::format("Entering redLightState, tested states: {}", m_testedStates));
                return true;
            },
            [this]() -> bool {
                m_stateCallOrder += "rl";
                nbn::log::info("Leaving redLightState");
                return true;
            });
    }

    void checkTestedStates() {
        unit_tests::equal(R"(m_testedStates should be 3)", static_cast<size_t>(3), m_testedStates);
        unit_tests::equal(R"(m_stateCallOrder should be "glyeylrerl")", std::string("glyeylrerl"), m_stateCallOrder);
    }

   protected:
    auto greenLightState() -> std::shared_ptr<fsm::interfaces::IState> { return m_spGreenLightState; }
    auto yellowLightState() -> std::shared_ptr<fsm::interfaces::IState> { return m_spYellowLightState; }
    auto redLightState() -> std::shared_ptr<fsm::interfaces::IState> { return m_spRedLightState; }

   private:
    size_t m_testedStates{0};
    std::string m_stateCallOrder;
    std::shared_ptr<fsm::interfaces::IState> m_spGreenLightState{nullptr};
    std::shared_ptr<fsm::interfaces::IState> m_spYellowLightState{nullptr};
    std::shared_ptr<fsm::interfaces::IState> m_spRedLightState{nullptr};
};

class RedLightCycle_ConditionalTransitions : public RedLightCycle {
    size_t m_testedTransitions{0};
    std::string m_transitionsCallOrder;

    auto name() -> property_ptr<std::string> { return property<std::string>(); }

   public:
    RedLightCycle_ConditionalTransitions() {
        name()->set("Red light cycle with conditional transitions");

        // Add transitions
        addTransition(
            greenLightState(), yellowLightState(), []() { return true; },
            [this]() -> bool {
                m_transitionsCallOrder += "gy";
                ++m_testedTransitions;
                nbn::log::info("Transition greenLightState -> yellowLightState");
                return true;
            });
        addTransition(
            yellowLightState(), redLightState(), []() { return true; },
            [this]() -> bool {
                m_transitionsCallOrder += "yr";
                ++m_testedTransitions;
                nbn::log::info("Transition yellowLightState -> redLightState");
                return true;
            });
        addTransition(
            redLightState(), greenLightState(), []() { return true; },
            [this]() -> bool {
                m_transitionsCallOrder += "rg";
                ++m_testedTransitions;
                nbn::log::info("Transition redLightState -> greenLightState");
                return true;
            });
    }

    void checkTestedTransitions() {
        unit_tests::equal(R"(m_testedTransitions should be 3)", static_cast<size_t>(3), m_testedTransitions);
        unit_tests::equal(R"(m_transitionsCallOrder should be "gyyrrg")", std::string("gyyrrg"), m_transitionsCallOrder);
    }
};

void test_finite_state_machine_conditional_transition() {
    RedLightCycle_ConditionalTransitions fsm;
    fsm.run();
    fsm.wait();
    fsm.checkTestedStates();
    fsm.checkTestedTransitions();
}

class RedLightCycle_OnSignalTransitions : public RedLightCycle {
    size_t m_testedTransitions{0};
    std::string m_transitionsCallOrder;

   public:
    RedLightCycle_OnSignalTransitions(Timer& timer) {
        name()->set("Red light cycle with on signal transitions");

        // Add transitions
        addTransition(greenLightState(), yellowLightState(), timer.signalTimeout(), [this]() -> bool {
            m_transitionsCallOrder += "gy";
            ++m_testedTransitions;
            nbn::log::info("Transition greenLightState -> yellowLightState");
            return true;
        });

        addTransition(yellowLightState(), redLightState(), timer.signalTimeout(), [this]() -> bool {
            m_transitionsCallOrder += "yr";
            ++m_testedTransitions;
            nbn::log::info("Transition yellowLightState -> redLightState");
            return true;
        });

        addTransition(redLightState(), greenLightState(), timer.signalTimeout(), [this]() -> bool {
            m_transitionsCallOrder += "rg";
            ++m_testedTransitions;
            nbn::log::info("Transition redLightState -> greenLightState");
            return true;
        });
    }

    void checkTestedTransitions() {
        unit_tests::equal(R"(m_testedTransitions should be 3)", static_cast<size_t>(3), m_testedTransitions);
        unit_tests::equal(R"(m_transitionsCallOrder should be "gyyrrg")", std::string("gyyrrg"), m_transitionsCallOrder);
    }
};

void test_finite_state_machine_on_signal_transition() {
    const std::chrono::milliseconds TIMER_DURATION{100};
    const size_t TIMER_REPETITIONS{3};
    Timer timer{TIMER_DURATION, []() {}, TIMER_REPETITIONS};

    RedLightCycle_OnSignalTransitions fsm{timer};

    fsm.run();
    timer.run();
    timer.wait();
    fsm.wait();

    fsm.checkTestedStates();
    fsm.checkTestedTransitions();
}

void test_finite_state_machine_lifecycle_and_graph() {
    fsm::Machine machine{};
    const auto idle = machine.addState(kIdleStateName);
    const auto running = machine.addState(kRunningStateName);
    const auto signal = std::make_shared<Signal<>>("transition");
    machine.addTransition(idle, running, signal);

    const auto graph = machine.toMermaidGraph();
    unit_tests::isTrue("FSM graph should contain the graph declaration", graph.find("graph TD") != std::string::npos);
    unit_tests::isTrue("FSM graph should contain the idle state", graph.find(kIdleStateName) != std::string::npos);
    unit_tests::isTrue("FSM graph should contain the running state", graph.find(kRunningStateName) != std::string::npos);
    unit_tests::isFalse("FSM should not be running before run", machine.isRunning());
    unit_tests::isFalse("FSM should not have a stop request before run", machine.isStopRequested());

    machine.run();
    unit_tests::isTrue("FSM should be running after run", machine.isRunning());
    signal->emit();
    machine.stop();
    machine.wait();
    unit_tests::isFalse("FSM should not be running after stop", machine.isRunning());
}

void test_finite_state_machine_requires_an_initial_state() {
    fsm::Machine machine{};
    unit_tests::isThrowing<std::runtime_error>("FSM without an initial state should reject run", [&machine]() { machine.run(); });
}

void test_finite_state_machine_graph_includes_transition_labels() {
    fsm::Machine machine{};
    const auto idle = machine.addState(kIdleStateName);
    const auto running = machine.addState(kRunningStateName);
    machine.addTransition(idle, running, []() { return true; }, []() { return true; });

    const auto graph = machine.toMermaidGraph();
    unit_tests::isTrue("FSM graph should include transition labels", graph.find("onTransition") != std::string::npos);
}

void test_finite_state_machine_graph_covers_generated_names_and_unlabelled_transitions() {
    fsm::Machine machine{};
    const auto unnamed = machine.addState();
    const auto named = machine.addState(kRunningStateName);
    machine.addTransition(unnamed, named, []() { return true; });

    const auto graph = machine.toMermaidGraph();
    unit_tests::isTrue("FSM should generate a name for an unnamed state", graph.find("State_") != std::string::npos);
    unit_tests::isTrue("FSM graph should include an unlabelled transition", graph.find(" -->|| ") != std::string::npos);
}
