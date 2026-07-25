#pragma once

#include <concepts>
#include <memory>
#include <type_traits>

namespace nbn::core {

/**
 * @file common.h
 * @brief Shared type traits and concepts used by core containers.
 */

/**
 * @struct is_smart_ptr
 * @brief Indicates whether a type is a supported standard smart pointer.
 * @tparam T Type to inspect.
 */
template <typename T>
struct is_smart_ptr : std::false_type {};

template <typename T>
struct is_smart_ptr<std::unique_ptr<T>> : std::true_type {};

template <typename T>
struct is_smart_ptr<std::shared_ptr<T>> : std::true_type {};

/**
 * @concept concurrent_safe_c
 * @brief Constrains values that can be stored by the concurrent containers.
 *
 * A value must be copy-constructible, or be a supported smart pointer that is
 * move-constructible. References and void are not valid element types.
 * @tparam T Type to constrain.
 */
template <typename T>
concept concurrent_safe_c = (std::is_copy_constructible_v<T> || (is_smart_ptr<T>::value && std::is_move_constructible_v<T>)) &&
                            std::is_destructible_v<T> && !std::is_reference_v<T> && !std::is_void_v<T>;

}  // namespace nbn::core
