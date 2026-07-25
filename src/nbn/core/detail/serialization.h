#pragma once

#include <chrono>
#include <concepts>
#include <cstddef>
#include <format>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include <nbn/core/XRefUnorderedMap.h>
#include <nbn/core/interfaces/Interface.h>
#include <nbn/core/interfaces/core.h>

#ifndef nbn_export_core
#define nbn_export_core
#endif

namespace nbn::core {

/**
 * @file detail/serialization.h
 * @brief Serialization contracts and value/object/array serialization helpers.
 */

namespace interfaces {

/**
 * @struct ISerializable
 * @brief Interface for serializable runtime objects.
 */

struct nbn_export_core ISerializable : public Interface {
    [[nodiscard]] virtual auto serialize() const -> std::string = 0;
    virtual auto deserialize(std::string_view str) -> void = 0;
};

}  // namespace interfaces

namespace detail {

// Remove shared_ptr qualifier
template <typename T>
struct remove_shared_ptr {
    using type = T;
};

template <typename T>
struct remove_shared_ptr<std::shared_ptr<T>> {
    using type = T;
};

// Type trait to check if a type is a serializable pointer
template <typename T>
struct is_serializable_ptr : std::false_type {};

template <typename T>
struct is_serializable_ptr<std::shared_ptr<T>> : std::is_base_of<interfaces::ISerializable, T> {};

// Type trait to get the value type of a std::vector
template <typename T>
struct vector_value_type;

// Specialization of the type trait for std::vector
template <typename ValueType>
struct vector_value_type<std::vector<ValueType>> {
    using type = ValueType;
};

// Helper type alias
template <typename T>
using vector_value_type_t = typename vector_value_type<T>::type;

}  // namespace detail

template <typename T>
using is_serializable_ptr = detail::is_serializable_ptr<typename std::remove_cv<T>::type>;

using serializable_ptr_t = std::shared_ptr<interfaces::ISerializable>;

template <typename T>
using vector_value_type_t = detail::vector_value_type_t<T>;

template <typename T>
struct enum_serialization {};

template <typename TDerived, typename TEnum>
struct enum_serialization_xref {
   private:
    friend TDerived;

    enum_serialization_xref(std::initializer_list<std::tuple<TEnum, std::string_view>> entries) {
        auto& map = rawValues();
        for (const auto& [value, name] : entries) {
            map.insert(value, name);
        }
    }

   public:
    static auto toString(TEnum value) -> std::string_view {
        const auto& map = values();
        if (!map.containsKey(value)) {
            throw std::runtime_error(std::format("Enum value '{}' is not mapped", static_cast<int>(value)));
        }
        return map.getValue(value);
    }

    static auto fromString(std::string_view value) -> TEnum {
        const auto& map = values();
        if (!map.containsValue(value)) {
            throw std::runtime_error(std::format("Enum string '{}' is not mapped", value));
        }
        return map.getKey(value);
    }

   private:
    static auto rawValues() -> XRefUnorderedMap<TEnum, std::string_view>& { return sValues; }

    static auto values() -> XRefUnorderedMap<TEnum, std::string_view>& {
        auto& map = rawValues();
        // Re-register the compile-time enum mapping on every access. This keeps
        // header-defined mappings valid when the template is instantiated in
        // both a shared library and its client executable.
        const TDerived initializer{};
        (void)initializer;
        return map;
    }

    inline static XRefUnorderedMap<TEnum, std::string_view> sValues{};
};

template <typename T>
concept enum_serializable_c = std::is_enum_v<T> && requires(T value, std::string_view text) {
    { enum_serialization<T>::toString(value) } -> std::convertible_to<std::string_view>;
    { enum_serialization<T>::fromString(text) } -> std::same_as<T>;
};

struct serializable_vector_t;

using serializable_variants_t = std::variant<char,
                                             unsigned char,
                                             short,
                                             unsigned short,
                                             int,
                                             unsigned int,
                                             long,
                                             unsigned long,
                                             long long,
                                             unsigned long long,
                                             float,
                                             double,
                                             long double,
                                             bool,
                                             std::nullptr_t,
                                             std::string,
                                             std::chrono::nanoseconds,
                                             serializable_ptr_t,
                                             serializable_vector_t>;

struct serializable_vector_t : public std::vector<serializable_variants_t> {
    using std::vector<serializable_variants_t>::vector;
};

template <typename T>
struct is_serializable_vector : std::false_type {};

template <typename T>
struct is_serializable_vector<std::vector<T>> : std::true_type {};

template <typename T>
struct is_serializable_vector<std::vector<std::shared_ptr<T>>> : std::is_base_of<interfaces::ISerializable, T> {};

template <typename T>
concept serializable_variant_c = [] {
    // Check if the type is not const-qualified or a reference.
    if constexpr (std::is_const_v<std::remove_reference_t<T>> || std::is_reference_v<T>) {
        static_assert(!std::is_const_v<std::remove_reference_t<T>>, "serializable_variant_c cannot be const-qualified");
        static_assert(!std::is_reference_v<T>, "serializable_variant_c cannot be a reference");
        return false;
    }
    return std::disjunction_v<
        std::is_same<T, char>, std::is_same<T, unsigned char>, std::is_same<T, short>, std::is_same<T, unsigned short>,
        std::is_same<T, int>, std::is_same<T, unsigned int>, std::is_same<T, long>, std::is_same<T, unsigned long>,
        std::is_same<T, long long>, std::is_same<T, unsigned long long>, std::is_same<T, float>, std::is_same<T, double>,
        std::is_same<T, long double>, std::is_same<T, bool>, std::is_same<T, std::nullptr_t>, std::is_same<T, std::string>,
        std::is_same<T, std::chrono::nanoseconds>, is_serializable_ptr<T>, std::is_base_of<interfaces::ISerializable, T>,
        is_serializable_vector<T>, std::bool_constant<enum_serializable_c<T>>>;
}();

/**
 * @brief Concept for signal/slot template argument types.
 *
 * Extends serializable_variant_c to additionally permit const-qualified and
 * lvalue-reference-qualified forms of any serializable base type, enabling
 * handlers to receive arguments as const-ref or mutable-ref. Rvalue
 * references are rejected because they cannot be safely forwarded across an
 * async emit boundary.
 */
template <typename T>
concept slot_arg_c = [] {
    if constexpr (std::is_rvalue_reference_v<T>) {
        static_assert(!std::is_rvalue_reference_v<T>, "slot_arg_c: rvalue references are not supported");
        return false;
    }
    return serializable_variant_c<std::remove_cvref_t<T>>;
}();

namespace serialization {

namespace json {

namespace constants {

constexpr auto TRUE{"true"};
constexpr auto FALSE{"false"};
constexpr auto NULL_PTR{"null"};

}  // namespace constants

nbn_export_core auto escapeString(std::string_view value) -> std::string;
nbn_export_core auto validateInputSize(std::string_view json) -> void;
nbn_export_core auto validateEnd(std::string_view json, size_t pos) -> void;
nbn_export_core auto parseString(std::string_view json, size_t& pos) -> std::string;
nbn_export_core auto beautifyJson(std::string_view json) -> std::string;
nbn_export_core auto deBeautifyJson(std::string_view json) -> std::string;
nbn_export_core auto parseObject(std::unordered_map<std::string, std::string>& propertiesAsString,
                                 std::string_view json,
                                 size_t& pos) -> void;
}  // namespace json

namespace object {

nbn_export_core auto serialize(interfaces::ISerializable* value) -> std::string;

nbn_export_core auto deserialize(std::string_view str) -> serializable_ptr_t;

template <typename T>
auto deserialize(std::string_view str) -> T {
    return std::dynamic_pointer_cast<typename detail::remove_shared_ptr<T>::type>(deserialize(str));
}

}  // namespace object

namespace value {

template <typename T>
auto serialize(const T& value) -> std::string;

template <>
auto serialize<serializable_variants_t>(const serializable_variants_t& value) -> std::string;

template <typename T>
auto deserialize(std::string_view str) -> T;

template <typename T>
    requires enum_serializable_c<T>
auto serialize(const T& value) -> std::string {
    return std::format(R"("{}")", json::escapeString(enum_serialization<T>::toString(value)));
}

template <typename T>
    requires enum_serializable_c<T>
auto deserialize(std::string_view str) -> T {
    size_t pos{0};
    const auto value = json::parseString(str, pos);
    json::validateEnd(str, pos);
    return enum_serialization<T>::fromString(value);
}

}  // namespace value

namespace array {

template <typename T>
auto serialize(const T& value) -> std::string;

template <typename T>
auto deserialize(std::string_view str) -> T;

extern template auto deserialize<std::vector<char>>(std::string_view str) -> std::vector<char>;
extern template auto deserialize<std::vector<unsigned char>>(std::string_view str) -> std::vector<unsigned char>;
extern template auto deserialize<std::vector<short>>(std::string_view str) -> std::vector<short>;
extern template auto deserialize<std::vector<unsigned short>>(std::string_view str) -> std::vector<unsigned short>;
extern template auto deserialize<std::vector<int>>(std::string_view str) -> std::vector<int>;
extern template auto deserialize<std::vector<unsigned int>>(std::string_view str) -> std::vector<unsigned int>;
extern template auto deserialize<std::vector<long>>(std::string_view str) -> std::vector<long>;
extern template auto deserialize<std::vector<unsigned long>>(std::string_view str) -> std::vector<unsigned long>;
extern template auto deserialize<std::vector<long long>>(std::string_view str) -> std::vector<long long>;
extern template auto deserialize<std::vector<unsigned long long>>(std::string_view str) -> std::vector<unsigned long long>;
extern template auto deserialize<std::vector<float>>(std::string_view str) -> std::vector<float>;
extern template auto deserialize<std::vector<double>>(std::string_view str) -> std::vector<double>;
extern template auto deserialize<std::vector<long double>>(std::string_view str) -> std::vector<long double>;
extern template auto deserialize<std::vector<bool>>(std::string_view str) -> std::vector<bool>;
extern template auto deserialize<std::vector<std::nullptr_t>>(std::string_view str) -> std::vector<std::nullptr_t>;
extern template auto deserialize<std::vector<std::string>>(std::string_view str) -> std::vector<std::string>;
extern template auto deserialize<std::vector<std::chrono::nanoseconds>>(std::string_view str)
    -> std::vector<std::chrono::nanoseconds>;
extern template auto deserialize<std::vector<serializable_ptr_t>>(std::string_view str) -> std::vector<serializable_ptr_t>;
extern template auto deserialize<std::vector<serializable_vector_t>>(std::string_view str) -> std::vector<serializable_vector_t>;

}  // namespace array

template <typename T>
[[nodiscard]] auto serialize(T value) -> std::string {
    if constexpr (is_serializable_vector<T>()) {
        return array::serialize<T>(value);
    } else if constexpr (is_serializable_ptr<T>()) {
        return serialization::object::serialize(value.get());
    } else {
        return serialization::value::serialize<T>(value);
    }
}

template <typename T>
auto deserialize(std::string_view str) -> T {
    json::validateInputSize(str);
    if constexpr (is_serializable_vector<T>()) {
        return array::deserialize<T>(str);
    } else if constexpr (is_serializable_ptr<T>()) {
        return serialization::object::deserialize<T>(str);
    } else {
        return serialization::value::deserialize<T>(str);
    }
}

}  // namespace serialization

}  // namespace nbn::core