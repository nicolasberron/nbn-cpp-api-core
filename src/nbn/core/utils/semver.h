#pragma once

#include <optional>
#include <string_view>

namespace nbn::core::utils::semver {

/**
 * @file semver.h
 * @brief Helpers for comparing dot-separated numeric version strings.
 */

/**
 * @brief Compares two versions numerically by component.
 * @return -1, 0, or 1; std::nullopt when either version is invalid.
 * @par Usage example
 * @code{.cpp}
 * if (nbn::core::utils::semver::isGreater("2.0.0", "1.9.0")) {
 *     // The newer API is available.
 * }
 * @endcode
 */
auto compare(std::string_view lhs, std::string_view rhs) -> std::optional<int>;

/** @brief Returns whether a version contains only numeric dot-separated components. */
auto isValid(std::string_view version) -> bool;

/** @brief Returns whether lhs is numerically less than rhs. */
auto isLess(std::string_view lhs, std::string_view rhs) -> bool;

/** @brief Returns whether lhs is less than or equal to rhs. */
auto isLessOrEqual(std::string_view lhs, std::string_view rhs) -> bool;

/** @brief Returns whether lhs and rhs represent the same version. */
auto isEqual(std::string_view lhs, std::string_view rhs) -> bool;

/** @brief Returns whether lhs is numerically greater than rhs. */
auto isGreater(std::string_view lhs, std::string_view rhs) -> bool;

/** @brief Returns whether lhs is greater than or equal to rhs. */
auto isGreaterOrEqual(std::string_view lhs, std::string_view rhs) -> bool;

}  // namespace nbn::core::utils::semver
