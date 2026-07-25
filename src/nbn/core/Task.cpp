#include <exception>

#include "Logger.h"
#include "Task.h"
#include "private/Application.h"
#include "private/Task.h"

namespace nbn::core {

// Factory registration
static auto const registerToFactory{[]() { return Object::factoryRegister<Task>(); }()};

Task::Task() : m_spImpl{std::make_unique<Impl>(this)} {}

Task::Task(task_body_t f, RunMode mode) : m_spImpl{std::make_unique<Impl>(this)} {
    init(f, mode);
}

Task::~Task() = default;

auto Task::init(task_body_t f, RunMode mode) -> void {
    m_spImpl->init(f, mode);
}

[[nodiscard]] auto Task::getTaskBody() const -> task_body_t {
    return m_spImpl->getTaskBody();
}

[[nodiscard]] auto Task::getRunMode() const -> RunMode {
    return m_spImpl->getRunMode();
}

void Task::run() {
    m_spImpl->run();
}

void Task::stop() {
    m_spImpl->stop();
}

[[nodiscard]] auto Task::isRunning() const -> bool {
    return m_spImpl->isRunning();
}

[[nodiscard]] auto Task::isStopRequested() const -> bool {
    return m_spImpl->isStopRequested();
}

void Task::wait() {
    m_spImpl->wait();
}

}  // namespace nbn::core
