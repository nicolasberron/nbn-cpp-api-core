#pragma once

#include <cstddef>
#include <functional>
#include <ranges>
#include <type_traits>
#include <vector>

#include "Application.h"
#include "Task.h"
#include "detail/serialization.h"
#include "private/format_bridge.h"

namespace nbn::core {

/**
 * @file UnitTests.h
 * @brief Built-in test runner and assertion helpers.
 */

/**
 * @class UnitTests
 * @brief Collects and executes named test functions.
 *
 * @par Usage example
 * @code{.cpp}
 * nbn::core::UnitTests tests;
 * tests.addTest("addition", [] {
 *     nbn::core::unit_tests::equal("1 + 1", 2, 1 + 1);
 * });
 *
 * tests.run();
 * tests.wait();
 * @endcode
 */

class nbn_export_core UnitTests : public Task {
   public:
    /** @brief Constructs an empty test runner. */
    UnitTests();
    /** @brief Destroys the test runner. */
    ~UnitTests() override;

    /**
     * @brief Adds a named test function to the runner.
     * @param name Display name of the test.
     * @param func Function invoked when the test runner executes.
     */
    void addTest(std::string name, std::function<void()> func);

   private:
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};
};

/**
 * @namespace unit_tests
 * @brief Assertion helper functions for core unit tests.
 */
namespace unit_tests {

namespace detail {

template <typename Left, typename Right>
[[nodiscard]] auto areEqual(const Left& left, const Right& right) -> bool {
    if constexpr (std::is_array_v<Left> && std::is_array_v<Right>) {
        return std::ranges::equal(left, right);
    } else {
        return std::equal_to<>{}(left, right);
    }
}

template <typename T>
    requires std::is_enum_v<T> && enum_serializable_c<T>
auto enumToTestString(const T& value) -> std::string {
    return std::string(enum_serialization<T>::toString(value));
}

template <typename T>
    requires std::is_enum_v<T> && (!enum_serializable_c<T>)
auto enumToTestString(const T& value) -> std::string {
    using underlying_t = std::underlying_type_t<T>;
    return std::format("{}", static_cast<underlying_t>(value));
}

}  // namespace detail

/**
 * @brief Converts a test value to the representation used in assertion messages.
 * @tparam T Value type.
 * @param value Value to format.
 * @return A readable string representation.
 */
template <typename T>
auto toTestString(const T& value) -> std::string {
    using value_t = std::remove_cvref_t<T>;

    if constexpr (is_serializable_vector<value_t>::value) {
        auto result = std::string{"["};
        auto isFirst = true;
        for (const auto& item : value) {
            if (!isFirst) {
                result += ", ";
            }
            result += toTestString(item);
            isFirst = false;
        }
        result += "]";
        return result;
    } else if constexpr (std::is_enum_v<value_t>) {
        return detail::enumToTestString(value);
    } else {
        return std::format("{}", value);
    }
}

/**
 * @brief Asserts that two values compare equal.
 * @param message Description shown when the assertion fails.
 * @param expected Expected value.
 * @param actual Actual value.
 */
template <typename Expected, typename Actual>
void equal(std::string_view message,
           const Expected& expected,
           const Actual& actual,
           [[maybe_unused]] const std::source_location location = std::source_location::current()) {
    if constexpr (std::is_same_v<std::remove_cvref_t<Expected>, std::nullptr_t> &&
                  std::is_same_v<std::remove_cvref_t<Actual>, std::nullptr_t>) {
        return;
    } else if (!detail::areEqual(expected, actual)) {
        nbn::log::fatal(std::format("FAILED: {}. Expected {}, but got {}.", message, toTestString(expected), toTestString(actual)));
    }
}

/**
 * @brief Asserts that two values compare different.
 * @param message Description shown when the assertion fails.
 * @param expected Value that must differ from actual.
 * @param actual Value being checked.
 */
template <typename Expected, typename Actual>
void notEqual(std::string_view message,
              const Expected& expected,
              const Actual& actual,
              [[maybe_unused]] const std::source_location location = std::source_location::current()) {
    if (detail::areEqual(expected, actual)) {
        nbn::log::fatal(
            std::format("FAILED: {}. Expected {} to be different from {}.", message, toTestString(expected), toTestString(actual)));
    }
}

/**
 * @brief Asserts that a value converts to true.
 * @param message Description shown when the assertion fails.
 * @param actual Value being checked.
 */
template <typename Actual>
void isTrue(std::string_view message,
            const Actual& actual,
            [[maybe_unused]] const std::source_location location = std::source_location::current()) {
    if (!actual) {
        nbn::log::fatal(std::format("FAILED: {}. Expected true, but got false.", message));
    }
}

/**
 * @brief Asserts that a value converts to false.
 * @param message Description shown when the assertion fails.
 * @param actual Value being checked.
 */
template <typename Actual>
void isFalse(std::string_view message,
             const Actual& actual,
             [[maybe_unused]] const std::source_location location = std::source_location::current()) {
    if (actual) {
        nbn::log::fatal(std::format("FAILED: {}. Expected false, but got true.", message));
    }
}

/**
 * @brief Asserts that invoking a callable throws the requested exception type.
 * @tparam Exception Expected exception type.
 * @param message Description shown when the assertion fails.
 * @param expr Callable to invoke.
 * @par Usage example
 * @code{.cpp}
 * nbn::core::unit_tests::isThrowing<std::runtime_error>("invalid input", [] {
 *     throw std::runtime_error{"invalid input"};
 * });
 * @endcode
 */
template <typename Exception>
void isThrowing(std::string_view message,
                const std::function<void()>& expr,
                [[maybe_unused]] const std::source_location location = std::source_location::current()) {
    try {
        expr();
        nbn::log::fatal(std::format("FAILED: {}. Expected exception of type {}, but no exception was thrown.", message,
                                    typeid(Exception).name()));
    } catch (...) {
        // Do nothing, exception of the expected type was thrown.
        return;
    }
}

}  // namespace unit_tests

}  // namespace nbn::core
