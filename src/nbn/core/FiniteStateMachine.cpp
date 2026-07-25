/*
 * FiniteStateMachine.cpp
 * Description:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more detail.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2023 Nicolas Berron <nicolas.berron@gmail.com>
 */

#include <tuple>
#include <unordered_map>

#include "Application.h"
#include "ConcurrentQueue.h"
#include "FiniteStateMachine.h"
#include "Ring.h"
#include "private/Thread.h"
#include "private/format_bridge.h"

namespace nbn::core::fsm {

// Forward declarations

class Transition;

namespace interfaces {

struct IMachineImpl : public ::nbn::core::interfaces::Interface {
   public:
    virtual auto isStopRequested() const -> bool = 0;
    virtual auto isStateCurrent(const std::shared_ptr<interfaces::IState>& spState) const -> bool = 0;
    virtual auto triggerTransition(const std::shared_ptr<Transition>& spTransition) -> void = 0;
};

}  // namespace interfaces

// fsm::State class implementation

struct State : public Object, public interfaces::IState, public std::enable_shared_from_this<State> {
    interfaces::IMachineImpl* m_pMachineImpl{nullptr};
    slot_t m_onEnter{nullptr};
    slot_t m_onLeave{nullptr};

    using signal_transition_t = std::tuple<std::string, std::string, std::string>;
    std::vector<signal_transition_t> m_signalTransitions;

    explicit State(interfaces::IMachineImpl* pMachineImpl,
                   std::string iName = "",
                   const slot_t& onEnter = nullptr,  // NOLINT(bugprone-easily-swappable-parameters)
                   const slot_t& onLeave = nullptr)
        : m_pMachineImpl{pMachineImpl}, m_onEnter{onEnter}, m_onLeave{onLeave} {
        name()->set(iName.empty() ? ::nbn::core::format("State_{}", Object::getUuid()) : iName);
    }

    ~State() override = default;

    // Properties
    auto name() -> property_ptr<std::string> { return property<std::string>(); }

    auto addsignal_transition_t(std::shared_ptr<interfaces::IState> spToState, signal_ptr<> onSignal, slot_t onTransition = nullptr)
        -> void {
        onSignal->connect([this, spToState, onTransition]() {
            if (m_pMachineImpl->isStateCurrent(shared_from_this()) && !m_pMachineImpl->isStopRequested()) {
                m_pMachineImpl->triggerTransition(
                    std::make_shared<Transition>(shared_from_this(), std::move(spToState), std::move(onTransition)));
            }
        });
        m_signalTransitions.emplace_back(name()->get(), dynamic_cast<State*>(spToState.get())->name()->get(), onSignal->getName());
    }

    auto onEnter() -> bool override { return m_onEnter != nullptr ? m_onEnter() : true; }

    auto onLeave() -> bool override { return m_onLeave != nullptr ? m_onLeave() : true; }
};

// fsm::Transition class implementation

class Transition {
   public:
    using slot_t = interfaces::IState::slot_t;

    std::shared_ptr<interfaces::IState> m_spFromState{nullptr};
    std::shared_ptr<interfaces::IState> m_spToState{nullptr};
    slot_t m_onTransition{nullptr};
    Machine::condition_t m_condition{nullptr};

    explicit Transition(std::shared_ptr<interfaces::IState> spFromState,
                        std::shared_ptr<interfaces::IState> spToState,
                        const slot_t& onTransition)
        : Transition(std::move(spFromState), std::move(spToState), onTransition, {}) {}

    Transition(std::shared_ptr<interfaces::IState> spFromState,
               std::shared_ptr<interfaces::IState> spToState,
               const slot_t& onTransition,  // NOLINT(bugprone-easily-swappable-parameters)
               const Machine::condition_t& condition)
        : m_spFromState{std::move(spFromState)},
          m_spToState{std::move(spToState)},
          m_onTransition{onTransition},
          m_condition{condition} {}

    ~Transition() = default;
};

// fsm::Machine::Impl class implementation

class Machine::Impl : public Thread, public ::nbn::core::fsm::interfaces::IMachineImpl {
    std::vector<std::shared_ptr<State>> m_states;
    std::vector<std::shared_ptr<Transition>> m_transitions;
    std::shared_ptr<interfaces::IState> m_spInitialState{nullptr};
    std::shared_ptr<interfaces::IState> m_spCurrentState{nullptr};
    Ring<std::shared_ptr<Transition>> m_conditionalTransitionsRingBuffer;

   public:
    Impl() : Thread{[this]() { fsmLoop(); }, Task::RunMode::Loop} {}

    void run() override {
        ::nbn::log::fatal_if(m_spInitialState == nullptr, "No initial state defined");

        ::nbn::log::debug(::nbn::core::format("FSM graph:\n{}", toMermaidGraph()));

        m_spCurrentState = m_spInitialState;

        Thread::run();
    }

    auto isStopRequested() const -> bool override { return Thread::isStopRequested(); }

    auto isStateCurrent(const std::shared_ptr<interfaces::IState>& spState) const -> bool override {
        return spState == m_spCurrentState;
    }

    auto triggerTransition(const std::shared_ptr<Transition>& spTransition) -> void override {
        std::array<slot_t, 3> slots{[spTransition]() -> bool { return spTransition->m_spFromState->onLeave(); },
                                    spTransition->m_onTransition,
                                    [spTransition]() -> bool { return spTransition->m_spToState->onEnter(); }};

        auto shouldBeStopped{false};
        for (const auto& slot : slots) {
            if (slot != nullptr) {
                if (!slot()) {
                    shouldBeStopped = true;
                    break;
                }
            }
        }

        m_spCurrentState = spTransition->m_spToState;

        if (shouldBeStopped) {
            stop();
        }
    }

    auto addState(const std::shared_ptr<State>& spState) -> std::shared_ptr<State>& {
        if (m_states.empty()) {
            m_spInitialState = spState;
        }
        m_states.emplace_back(spState);
        return m_states.back();
    }

    auto addTransition(std::shared_ptr<interfaces::IState> spFromState,
                       std::shared_ptr<interfaces::IState> spToState,
                       signal_ptr<> onSignal,
                       slot_t onTransition = nullptr) -> void {
        std::dynamic_pointer_cast<State>(spFromState)
            ->addsignal_transition_t(std::move(spToState), std::move(onSignal), std::move(onTransition));
    }

    auto addTransition(std::shared_ptr<interfaces::IState> spFromState,
                       std::shared_ptr<interfaces::IState> spToState,
                       condition_t condition,
                       slot_t onTransition = nullptr) -> void {
        auto spTransition{std::make_shared<Transition>(std::move(spFromState), std::move(spToState), std::move(onTransition),
                                                       std::move(condition))};
        m_transitions.emplace_back(spTransition);
        m_conditionalTransitionsRingBuffer.push_back(std::move(spTransition));
    }

    [[nodiscard]] auto toMermaidGraph() -> std::string {
        std::string graph{"graph TD\n"};

        // Add states
        std::unordered_map<std::shared_ptr<interfaces::IState>, std::string> stateNames;
        for (const auto& spState : m_states) {
            auto stateName{spState->name()->get()};
            stateNames[spState] = stateName;
            graph += ::nbn::core::format("    {}[{}]\n", stateName, stateName);
        }

        // Add signal transitions
        for (const auto& spState : m_states) {
            for (const auto& [fromStateName, toStateName, signalName] :
                 std::dynamic_pointer_cast<State>(spState)->m_signalTransitions) {
                graph += ::nbn::core::format("    {} -->|{}| {}\n", fromStateName, signalName, toStateName);
            }
        }

        // Add conditional transitions
        for (const auto& spTransition : m_transitions) {
            auto fromStateName{stateNames[spTransition->m_spFromState]};
            auto toStateName{stateNames[spTransition->m_spToState]};
            graph += ::nbn::core::format("    {} -->|{}| {}\n", fromStateName,
                                         spTransition->m_onTransition != nullptr ? "onTransition" : "", toStateName);
        }

        return graph;
    }

    auto fsmLoop() -> void {
        std::shared_ptr<Transition> spConditionalTransition{nullptr};
        if (m_conditionalTransitionsRingBuffer.get(spConditionalTransition)) {
            if (spConditionalTransition->m_spFromState == m_spCurrentState && spConditionalTransition->m_condition()) {
                triggerTransition(spConditionalTransition);
            }
        }
    }
};

// fsm::Machine class implementation

Machine::Machine() : m_spImpl(std::make_unique<Machine::Impl>()) {
    m_spImpl->signalStarted()->connect([this]() { signalStarted()->emit(); });
    m_spImpl->signalStopped()->connect([this]() { signalStopped()->emit(); });
}

Machine::~Machine() = default;

auto Machine::addState(std::string name, const slot_t& onEnter, const slot_t& onLeave) -> std::shared_ptr<interfaces::IState> {
    return m_spImpl->addState(std::make_shared<State>(m_spImpl.get(), std::move(name), onEnter, onLeave));
}

auto Machine::addTransition(std::shared_ptr<interfaces::IState> spFromState,
                            std::shared_ptr<interfaces::IState> spToState,
                            signal_ptr<> onSignal,
                            slot_t onTransition) -> void {
    m_spImpl->addTransition(std::move(spFromState), std::move(spToState), std::move(onSignal), std::move(onTransition));
}

auto Machine::addTransition(std::shared_ptr<interfaces::IState> spFromState,
                            std::shared_ptr<interfaces::IState> spToState,
                            condition_t condition,
                            slot_t onTransition) -> void {
    m_spImpl->addTransition(std::move(spFromState), std::move(spToState), std::move(condition), std::move(onTransition));
}

[[nodiscard]] auto Machine::toMermaidGraph() -> std::string {
    return m_spImpl->toMermaidGraph();
}

auto Machine::run() -> void {
    m_spImpl->run();
}

auto Machine::stop() -> void {
    m_spImpl->stop();
}

[[nodiscard]] auto Machine::isRunning() const -> bool {
    return m_spImpl->isRunning();
}

[[nodiscard]] auto Machine::isStopRequested() const -> bool {
    return m_spImpl->isStopRequested();
}

auto Machine::wait() -> void {
    m_spImpl->wait();
}

}  // namespace nbn::core::fsm
