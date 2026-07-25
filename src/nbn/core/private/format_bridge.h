#pragma once

#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#if !defined(PLATFORMIO_BUILD)
#include <format>
#endif

namespace nbn::core {

#if defined(PLATFORMIO_BUILD)
namespace detail {

template <typename T>
auto to_formatted_string(std::string_view spec, T&& value) -> std::string {
    std::ostringstream oss;
    using value_t = std::remove_cv_t<std::remove_reference_t<T>>;

    if constexpr (std::is_integral_v<value_t>) {
        if (spec == "X" || spec == "x") {
            if (spec == "X") {
                oss << std::uppercase;
            }
            oss << std::hex;
        }
    }

    oss << std::forward<T>(value);
    return oss.str();
}

inline auto append_escaped(std::string& out, std::string_view str) -> void {
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '{' && i + 1U < str.size() && str[i + 1U] == '{') {
            out.push_back('{');
            ++i;
            continue;
        }
        if (str[i] == '}' && i + 1U < str.size() && str[i + 1U] == '}') {
            out.push_back('}');
            ++i;
            continue;
        }
        out.push_back(str[i]);
    }
}

inline auto find_next_placeholder(std::string& out, std::string_view fmt, size_t start, size_t& open_pos, std::string_view& spec)
    -> bool {
    for (size_t i = start; i < fmt.size(); ++i) {
        if (fmt[i] == '{') {
            if (i + 1U < fmt.size() && fmt[i + 1U] == '{') {
                out.append(fmt.substr(start, i - start));
                out.push_back('{');
                return find_next_placeholder(out, fmt, i + 2U, open_pos, spec);
            }

            const size_t close = fmt.find('}', i + 1U);
            if (close == std::string_view::npos) {
                out.append(fmt.substr(start));
                return false;
            }

            out.append(fmt.substr(start, i - start));
            auto token = fmt.substr(i + 1U, close - i - 1U);
            spec = (!token.empty() && token[0] == ':') ? token.substr(1U) : std::string_view{};
            open_pos = i;
            return true;
        }

        if (fmt[i] == '}' && i + 1U < fmt.size() && fmt[i + 1U] == '}') {
            out.append(fmt.substr(start, i - start));
            out.push_back('}');
            return find_next_placeholder(out, fmt, i + 2U, open_pos, spec);
        }
    }

    out.append(fmt.substr(start));
    return false;
}

inline auto format_impl(std::string& out, std::string_view fmt, size_t start) -> void {
    append_escaped(out, fmt.substr(start));
}

template <typename Arg, typename... Rest>
auto format_impl(std::string& out, std::string_view fmt, size_t start, Arg&& arg, Rest&&... rest) -> void {
    size_t open_pos = 0;
    std::string_view spec{};
    if (!find_next_placeholder(out, fmt, start, open_pos, spec)) {
        return;
    }

    const size_t close = fmt.find('}', open_pos + 1U);
    out += to_formatted_string(spec, std::forward<Arg>(arg));
    format_impl(out, fmt, close + 1U, std::forward<Rest>(rest)...);
}

}  // namespace detail

template <typename... Args>
auto format(std::string_view fmt, Args&&... args) -> std::string {
    std::string out;
    out.reserve(fmt.size() + (sizeof...(args) * 8U));
    detail::format_impl(out, fmt, 0U, std::forward<Args>(args)...);
    return out;
}

#else

using std::format;

#endif

}  // namespace nbn::core

#if defined(PLATFORMIO_BUILD)
namespace std {

template <typename... Args>
inline auto format(std::string_view fmt, Args&&... args) -> std::string {
    return ::nbn::core::format(fmt, std::forward<Args>(args)...);
}

}  // namespace std
#endif
