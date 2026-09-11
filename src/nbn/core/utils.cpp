#include <array>
#include <memory>
#include <mutex>
#include <random>
#if !defined(PLATFORMIO_BUILD)
#include <regex>
#endif
#include <sstream>

#if defined(PLATFORMIO_BUILD)
#include <esp_system.h>
#endif

#if defined(_MSC_VER)
#include <dbghelp.h>
#include <windows.h>
#include <array>
#include <iostream>
#include <typeinfo>
#else
#include <cxxabi.h>
#include <typeinfo>
#endif

#include "Application.h"
#include "private/format_bridge.h"
#include "utils.h"

namespace nbn::core::utils {

namespace abi {
[[nodiscard]] auto demangle(std::string_view symbol) -> std::string
#if defined(_MSC_VER)
{
    const auto bufferSize = 1024;
    std::array<char, bufferSize> buffer{};
    DWORD length = UnDecorateSymbolName(symbol.c_str(), buffer.data(), buffer.size(), UNDNAME_COMPLETE);

    if (length == 0) {
        std::cerr << "Demangling failed: " << GetLastError() << '\n';
        return symbol;
    } else {
        return {buffer.data(), static_cast<std::size_t>(length)};
    }
}
#else
{
    int status = -1;
    const std::string symbolString{symbol};
    std::unique_ptr<char, void (*)(void*)> result(::abi::__cxa_demangle(symbolString.c_str(), nullptr, nullptr, &status),
                                                  std::free);
    // __cxa_demangle() returns the original symbol for invalid ABI input; verify that path explicitly in tests.
    return (status == 0) ? std::move(std::string(result.get())) : std::move(std::string(symbol));
}
#endif  // defined(_MSC_VER)

[[nodiscard]] auto getFunctionName(std::string_view functionSignature) -> std::string {
#if defined(PLATFORMIO_BUILD)
    std::string name(functionSignature);

    auto open_paren = name.rfind('(');
    if (open_paren != std::string::npos) {
        name.erase(open_paren);
    }

    auto last_space = name.find_last_of(' ');
    if (last_space != std::string::npos) {
        name = name.substr(last_space + 1);
    }

    auto last_scope = name.rfind("::");
    if (last_scope != std::string::npos) {
        name = name.substr(last_scope + 2);
    }

    return name;
#else
    std::string name(functionSignature);

    // Equivalent to stripping ^.* , ^.*: and (.*$ — but without constructing std::regex on every
    // Much faster than regex, and avoids regex issues on some platforms (e.g. ESP32)
    if (const auto open_paren = name.rfind('('); open_paren != std::string::npos) {
        name.erase(open_paren);
    }
    if (const auto last_scope = name.rfind("::"); last_scope != std::string::npos) {
        name = name.substr(last_scope + 2U);
    } else if (const auto last_space = name.find_last_of(' '); last_space != std::string::npos) {
        name = name.substr(last_space + 1U);
    }
    return name;
#endif
}

template <>
auto getTypeName<std::string>() -> std::string {
    // Returning with generic function can give std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >
    // instead of std::string
    return "std::string";
}

}  // namespace abi

namespace uuid {
auto generate() -> std::string {
    constexpr auto zero{0};
    constexpr auto fifteen{15};
    constexpr auto eight{8};
    constexpr auto eleven{11};

    constexpr int num_segments = 5;
    constexpr std::array<int, num_segments> segment_lengths = {8, 4, 3, 3, 12};

#if defined(PLATFORMIO_BUILD)
    static std::mt19937 gen(static_cast<uint32_t>(esp_random()));
#else
    static std::random_device rd;
    static std::mt19937 gen(rd());
#endif
    static std::mutex generatorMutex;
    const std::lock_guard lock{generatorMutex};
    static std::uniform_int_distribution<> dis(zero, fifteen);
    static std::uniform_int_distribution<> dis2(eight, eleven);

    std::stringstream ss;
    ss << std::hex;

    int i = 0;
    for (const auto& segment_length : segment_lengths) {
        if (i > 0) {
            ss << "-";
        }

        if (i == 2) {
            ss << "4";
        } else if (i == 3) {
            ss << dis2(gen);
        }

        for (int j{0}; j < segment_length; ++j) {
            ss << dis(gen);
        }

        ++i;
    }

    return ss.str();
}
}  // namespace uuid

namespace string {

std::string toLower(std::string_view str) {
    std::string lowerStr{str};
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

std::string toUpper(std::string_view str) {
    std::string upperStr{str};
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), [](unsigned char c) { return std::toupper(c); });
    return upperStr;
}

// Types to string

template <>
std::string toString<bool>(const bool& value) {
    return value ? serialization::json::constants::TRUE : serialization::json::constants::FALSE;
}

template <>
std::string toString<char>(const char& value) {
    return std::string(1, value);
}

template <>
std::string toString<std::chrono::nanoseconds>(const std::chrono::nanoseconds& value) {
    return std::to_string(value.count());
}

template <>
std::string toString<std::nullptr_t>(const std::nullptr_t&) {
    return serialization::json::constants::NULL_PTR;
}

// String to types

template <>
bool fromString<bool>(std::string_view str) {
    if (str == serialization::json::constants::TRUE) {
        return true;
    } else if (str == serialization::json::constants::FALSE) {
        return false;
    }
    nbn::log::fatal(std::format("Cannot convert string '{}' to bool", str));
}

template <>
char fromString<char>(std::string_view str) {
    nbn::log::fatal_if(str.size() != 1, std::format("Cannot convert string '{}' to char", str));
    return str[0];
}

template <>
unsigned char fromString<unsigned char>(std::string_view str) {
    return static_cast<unsigned char>(std::stoi(std::string(str)));
}

template <>
float fromString<float>(std::string_view str) {
    return std::stof(std::string(str));
}

template <>
double fromString<double>(std::string_view str) {
    return std::stod(std::string(str));
}

template <>
long double fromString<long double>(std::string_view str) {
    return std::stold(std::string(str));
}

template <>
int fromString<int>(std::string_view str) {
    return std::stoi(std::string(str));
}

template <>
unsigned int fromString<unsigned int>(std::string_view str) {
    return std::stoul(std::string(str));
}

template <>
long fromString<long>(std::string_view str) {
    return std::stol(std::string(str));
}

template <>
unsigned long fromString<unsigned long>(std::string_view str) {
    return std::stoul(std::string(str));
}

template <>
long long fromString<long long>(std::string_view str) {
    return std::stoll(std::string(str));
}

template <>
unsigned long long fromString<unsigned long long>(std::string_view str) {
    return std::stoull(std::string(str));
}

template <>
std::chrono::nanoseconds fromString<std::chrono::nanoseconds>(std::string_view str) {
    long long value = std::stoll(std::string(str));
    return std::chrono::nanoseconds(value);
}

template <>
std::nullptr_t fromString<std::nullptr_t>(std::string_view str) {
    nbn::log::fatal_if(str != serialization::json::constants::NULL_PTR, std::format("Cannot convert string '{}' to nullptr", str));
    return nullptr;
}

}  // namespace string

}  // namespace nbn::core::utils