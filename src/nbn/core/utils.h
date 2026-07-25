#pragma once

#include <atomic>
#include <chrono>
#include <cmath>
#include <limits>
#include <string>

#include "interfaces/object.h"
#include "private/format_bridge.h"

namespace nbn::core {

/**
 * @file utils.h
 * @brief ABI, UUID, string conversion, and numeric utility functions.
 */

class Object;

namespace utils {

template <typename T>
struct is_atomic {
    static constexpr bool value = false;
};

template <typename T>
struct is_atomic<::std::atomic<T>> {
    static constexpr bool value = true;
};

namespace abi {

/** @brief Converts a compiler-mangled symbol name to a readable type name. */
[[nodiscard]] nbn_export_core auto demangle(std::string_view symbol) -> std::string;
/** @brief Extracts a function name from a compiler-generated signature. */
[[nodiscard]] nbn_export_core auto getFunctionName(std::string_view functionSignature) -> std::string;

/**
 * @brief Returns the demangled name of a type.
 * @tparam T Type whose compiler name should be converted.
 * @return A human-readable type name.
 * @par Usage example
 * @code{.cpp}
 * const auto name = nbn::core::utils::abi::getTypeName<int>();
 * @endcode
 */
template <typename T>
[[nodiscard]] nbn_export_core auto getTypeName() -> std::string {
    return demangle(typeid(T).name());
}

template <typename T>
[[nodiscard]] nbn_export_core auto getTypeName([[maybe_unused]] const T& t) -> std::string {
    return demangle(typeid(T).name());
}

template <>
[[nodiscard]] nbn_export_core auto getTypeName<std::string>() -> std::string;
extern template auto getTypeName<std::string>() -> std::string;

/**
 * @brief Returns the fully-qualified type name of T as a human-readable string.
 *
 * Decorates the demangled base-type name with any cv/reference qualifiers:
 * - const T       -> "const <base>"
 * - T&            -> "<base>&"
 * - const T&      -> "const <base>&"
 */
template <typename T>
[[nodiscard]] nbn_export_core auto qualifiedTypeName() -> std::string {
    auto name = getTypeName<std::remove_cvref_t<T>>();
    if constexpr (std::is_const_v<std::remove_reference_t<T>>) {
        name = "const " + name;
    }
    if constexpr (std::is_lvalue_reference_v<T>) {
        name += "&";
    } else if constexpr (std::is_rvalue_reference_v<T>) {
        name += "&&";
    }
    return name;
}

}  // namespace abi

namespace uuid {

/** @brief Generates a new UUID string. */
nbn_export_core auto generate() -> std::string;

}  // namespace uuid

namespace string {

/** @brief Converts a string to lowercase. */
std::string toLower(std::string_view str);

/** @brief Converts a string to uppercase. */
std::string toUpper(std::string_view str);

// Types to string

template <typename T>
    requires(!utils::is_atomic<T>::value)
std::string toString(const T& value) {
    return std::format("{}", value);
}

template <typename T>
    requires(utils::is_atomic<T>::value)
std::string toString(const T& value) {
    return toString(value.load());
}

template <typename T>
std::string toString(std::string_view value) {
    return std::string(value);
}

template <>
std::string toString<std::string>(std::string_view value);
extern template std::string toString<std::string>(std::string_view value);

template <>
std::string toString<bool>(const bool& value);
extern template std::string toString<bool>(const bool& value);

template <>
std::string toString<char>(const char& value);
extern template std::string toString<char>(const char& value);

template <>
std::string toString<std::chrono::nanoseconds>(const std::chrono::nanoseconds& value);
extern template std::string toString<std::chrono::nanoseconds>(const std::chrono::nanoseconds& value);

template <>
std::string toString<std::nullptr_t>(const std::nullptr_t&);
extern template std::string toString<std::nullptr_t>(const std::nullptr_t&);

// String to types

/**
 * @brief Parses a string into a value using stream extraction.
 * @tparam T Destination type.
 * @param str Text representation of the value.
 * @return The parsed value.
 */
template <typename T>
T fromString(std::string_view str) {
    T value;
    std::string strValue{str};
    std::istringstream iss(strValue);
    iss >> value;
    return value;
}

template <>
bool fromString<bool>(std::string_view str);
extern template bool fromString<bool>(std::string_view str);

template <>
char fromString<char>(std::string_view str);
extern template char fromString<char>(std::string_view str);

template <>
unsigned char fromString<unsigned char>(std::string_view str);
extern template unsigned char fromString<unsigned char>(std::string_view str);

template <>
float fromString<float>(std::string_view str);
extern template float fromString<float>(std::string_view str);

template <>
double fromString<double>(std::string_view str);
extern template double fromString<double>(std::string_view str);

template <>
long double fromString<long double>(std::string_view str);
extern template long double fromString<long double>(std::string_view str);

template <>
int fromString<int>(std::string_view str);
extern template int fromString<int>(std::string_view str);

template <>
unsigned int fromString<unsigned int>(std::string_view str);
extern template unsigned int fromString<unsigned int>(std::string_view str);

template <>
long fromString<long>(std::string_view str);
extern template long fromString<long>(std::string_view str);

template <>
unsigned long fromString<unsigned long>(std::string_view str);
extern template unsigned long fromString<unsigned long>(std::string_view str);

template <>
long long fromString<long long>(std::string_view str);
extern template long long fromString<long long>(std::string_view str);

template <>
unsigned long long fromString<unsigned long long>(std::string_view str);
extern template unsigned long long fromString<unsigned long long>(std::string_view str);

template <>
std::chrono::nanoseconds fromString<std::chrono::nanoseconds>(std::string_view str);
extern template std::chrono::nanoseconds fromString<std::chrono::nanoseconds>(std::string_view str);

template <>
std::nullptr_t fromString<std::nullptr_t>(std::string_view str);
extern template std::nullptr_t fromString<std::nullptr_t>(std::string_view str);

}  // namespace string

namespace maths {

/**
 * @brief Compares values, using epsilon tolerance for floating-point types.
 * @tparam T Comparable value type.
 */
template <typename T>
bool equal(const T& a, const T& b) {
    if constexpr (std::is_floating_point_v<T>) {
        return std::abs(a - b) <= std::numeric_limits<T>::epsilon();
    }
    return a == b;
}

}  // namespace maths

}  // namespace utils

}  // namespace nbn::core
