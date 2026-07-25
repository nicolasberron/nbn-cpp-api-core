/*
 * FiniteStateMachine.h
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

#pragma once

#include <functional>
#include <type_traits>

#include "Object.h"
#include "detail/Signal.h"
#include "interfaces/concurrency.h"

namespace nbn::core::fsm {

namespace interfaces {

/**
 * @struct IState
 * @brief The IState interface represents a state in a finite state machine.
 */
struct IState : public nbn::core::interfaces::Interface {
    /**
     * @brief The slot_t type represents a function that returns whether the fsm loop should continue.
     */
    using slot_t = std::function<bool()>;

    /**
     * @brief Returns the function to be called when entering this state.
     * @return Whether the fsm loop should continue.
     */
    virtual auto onEnter() -> bool = 0;

    /**
     * @brief Returns the function to be called when leaving this state.
     * @return Whether the fsm loop should continue.
     */
    virtual auto onLeave() -> bool = 0;
};

}  // namespace interfaces

/**
 * @class Machine
 * @brief The Machine class represents a Finite State Machine (FSM) that manages states and transitions.
 *
 * The FSM operates in a loop, continuously checking for available transitions and executing them.
 * Signal transitions have priority over conditional transitions.
 *
 * Available Transitions:
 * - Signal Transitions: Occur in response to a signal. Source state, target state, and signal are specified.
 * - Conditional Transitions: Occur based on a condition. Source state, target state, and condition are specified.
 *
 * Initial State:
 * - The initial state is the first state pushed to the FSM. When the machine is started, it will not enter the initial state: it
 * will *ALREADY* be in the initial state. This means that the onEnter() function of the initial state will not be called when the
 * machine is started. It will be called if a transition occurs that leads back to the initial state.
 *
 * @par Usage example
 * @code{.cpp}
 * nbn::core::fsm::Machine machine;
 *
 * auto idle = machine.addState("Idle");
 * auto running = machine.addState("Running");
 *
 * auto startSignal = std::make_shared<nbn::core::Signal<>>();
 * machine.addTransition(idle, running, startSignal);
 * machine.addTransition(running, idle, [] { return true; });
 *
 * machine.run();
 * startSignal->emit();
 * machine.stop();
 * machine.wait();
 * @endcode
 */
class nbn_export_core Machine : public Object, public ::nbn::core::interfaces::IRunnable {
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};

   public:
    /**
     * @brief The slot_t type represents a function that returns whether the fsm loop should continue.
     */
    using slot_t = std::function<bool()>;

    /**
     * @brief The Condition type represents a function that returns whether a condition is met.
     */
    using condition_t = std::function<bool()>;

    /**
     * @brief Constructs a Machine object.
     */
    Machine();

    /**
     * @brief Destructor for the Machine object.
     */
    ~Machine() override;

    // Signals
    /**
     * @brief Returns the signal that is emitted when the machine is started.
     * @return The signal that is emitted when the machine is started.
     */
    auto signalStarted() -> signal_ptr<> { return signal<>(); }

    /**
     * @brief Returns the signal that is emitted when the machine is stopped.
     * @return The signal that is emitted when the machine is stopped.
     */
    auto signalStopped() -> signal_ptr<> { return signal<>(); }

    /**
     * @brief Adds a state to the finite state machine.
     * @param name The name of the state. If not provided, the name will be "State_{object_uuid}",
     * @param onEnter The function to be called when entering this state.
     * @param onLeave The function to be called when leaving this state.
     * @return The shared pointer to the state.
     */
    auto addState(std::string name = "", const slot_t& onEnter = nullptr, const slot_t& onLeave = nullptr)
        -> std::shared_ptr<interfaces::IState>;

    /**
     * @brief Adds a transition between two states based on a signal.
     * @param spFromState The shared pointer to the source state of the transition.
     * @param spToState The shared pointer to the destination state of the transition.
     * @param onSignal The signal that triggers the transition.
     * @param onTransition The function to be called when the transition occurs.
     */
    auto addTransition(std::shared_ptr<interfaces::IState> spFromState,
                       std::shared_ptr<interfaces::IState> spToState,
                       signal_ptr<> onSignal,
                       slot_t onTransition = nullptr) -> void;

    /**
     * @brief Adds a transition between two states based on a condition.
     * @param spFromState The shared pointer to the source state of the transition.
     * @param spToState The shared pointer to the destination state of the transition.
     * @param condition The condition that must be satisfied for the transition to occur.
     * @param onTransition The function to be called when the transition occurs.
     */
    auto addTransition(std::shared_ptr<interfaces::IState> spFromState,
                       std::shared_ptr<interfaces::IState> spToState,
                       condition_t condition,
                       slot_t onTransition = nullptr) -> void;

    /**
     * @brief Creates a mermaid graph of the finite state machine.
     *
     * @return std::string The mermaid graph.
     */
    [[nodiscard]] auto toMermaidGraph() -> std::string;

    // IRunnable interface

    /**
     * @brief Runs the finite state machine.
     */
    auto run() -> void override;

    /**
     * @brief Stops the execution of the finite state machine.
     */
    auto stop() -> void override;

    /**
     * @brief Checks if the finite state machine is running.
     * @return True if the finite state machine is running, false otherwise.
     */
    [[nodiscard]] auto isRunning() const -> bool override;

    /**
     * @brief Checks if a stop has been requested for the finite state machine.
     * @return True if a stop has been requested, false otherwise.
     */
    [[nodiscard]] auto isStopRequested() const -> bool override;

    /**
     * @brief Waits for the finite state machine to complete its execution.
     */
    auto wait() -> void override;
};

}  // namespace nbn::core::fsm
