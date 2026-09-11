#include <algorithm>
#include <array>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iterator>
#include <limits>
#include <unordered_set>
#include <vector>

#include "../Object.h"
#include "../interfaces/concurrency.h"
#include "../utils.h"
#include "Application.h"
#include "Property.h"
#include "serialization.h"

namespace nbn::core::detail {

// Specialization of the type trait for serializable_vector_t
template <>
struct vector_value_type<serializable_vector_t> {
    using type = serializable_variants_t;
};

}  // namespace nbn::core::detail

namespace nbn::core::serialization {

namespace json {

// https://www.ecma-international.org/wp-content/uploads/ECMA-404_2nd_edition_december_2017.pdf

constexpr char QUOTE = '"';
constexpr char BACKSLASH = '\\';
constexpr char SLASH = '/';
constexpr char COMMA = ',';
constexpr char COLON = ':';
constexpr char OPEN_BRACE = '{';
constexpr char CLOSE_BRACE = '}';
constexpr char OPEN_BRACKET = '[';
constexpr char CLOSE_BRACKET = ']';
constexpr unsigned char JSON_CONTROL_CHARACTER_LIMIT = 0x20U;

constexpr uint32_t HEX_ALPHA_OFFSET = 10U;
constexpr uint32_t UTF8_MAX_1_BYTE = 0x7FU;
constexpr uint32_t UTF8_MAX_2_BYTES = 0x7FFU;
constexpr uint32_t UTF8_MAX_3_BYTES = 0xFFFFU;
constexpr uint32_t UTF8_MAX_CODEPOINT = 0x10FFFFU;
constexpr uint32_t UTF8_LEAD_2_BYTES = 0xC0U;
constexpr uint32_t UTF8_LEAD_3_BYTES = 0xE0U;
constexpr uint32_t UTF8_LEAD_4_BYTES = 0xF0U;
constexpr uint32_t UTF8_CONTINUATION = 0x80U;
constexpr uint32_t UTF8_MASK_6_BITS = 0x3FU;
constexpr uint32_t UTF8_MASK_5_BITS = 0x1FU;
constexpr uint32_t UTF8_MASK_4_BITS = 0x0FU;
constexpr uint32_t UTF8_MASK_3_BITS = 0x07U;
constexpr uint32_t SHIFT_6 = 6U;
constexpr uint32_t SHIFT_10 = 10U;
constexpr uint32_t SHIFT_12 = 12U;
constexpr uint32_t SHIFT_18 = 18U;
constexpr uint32_t HIGH_SURROGATE_START = 0xD800U;
constexpr uint32_t HIGH_SURROGATE_END = 0xDBFFU;
constexpr uint32_t LOW_SURROGATE_START = 0xDC00U;
constexpr uint32_t LOW_SURROGATE_END = 0xDFFFU;
constexpr uint32_t SURROGATE_BASE = 0x10000U;
constexpr size_t UNICODE_HEX_DIGITS = 4U;
constexpr size_t SURROGATE_SEQUENCE_TAIL_LEN = 6U;
constexpr size_t INITIAL_OBJECT_BUCKETS = 8U;
constexpr size_t MAX_INPUT_SIZE = static_cast<size_t>(16U) * 1024U * 1024U;
constexpr size_t MAX_NESTING_DEPTH = 256U;

auto maximumCollectionElements(std::string_view json) -> size_t {
    // Every JSON element needs at least one byte. The input-size-derived budget
    // keeps allocations proportional to the document while retaining the
    // existing 16 MiB hard input ceiling.
    return std::max(size_t{1}, json.size() / 2U + 1U);
}

auto maximumDecodedStringSize(std::string_view json) -> size_t {
    constexpr size_t kMaxExpansionFactor{4U};
    if (json.size() > MAX_INPUT_SIZE / kMaxExpansionFactor) {
        return MAX_INPUT_SIZE;
    }
    return std::max(size_t{1}, json.size() * kMaxExpansionFactor);
}

auto enforceCollectionLimit(std::string_view json, size_t count, std::string_view kind) -> void {
    nbn::log::fatal_if(count > maximumCollectionElements(json), std::format("Invalid JSON: maximum {} count exceeded.", kind));
}

auto maximumNestingDepth(std::string_view json) -> size_t {
    // Keeping the budget proportional to the document avoids granting tiny
    // inputs a needlessly large recursion budget while the private ceiling
    // protects the call stack.
    return std::min(MAX_NESTING_DEPTH, std::max(size_t{1}, json.size()));
}

thread_local size_t parseDepth{0};  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

class ParseDepthGuard final {
   public:
    explicit ParseDepthGuard(std::string_view json) {
        const auto maximumDepth = maximumNestingDepth(json);
        nbn::log::fatal_if(parseDepth >= maximumDepth,
                           std::format("Invalid JSON: maximum nesting depth of {} exceeded.", maximumDepth));
        ++parseDepth;
    }

    ~ParseDepthGuard() { --parseDepth; }

    ParseDepthGuard(const ParseDepthGuard&) = delete;
    auto operator=(const ParseDepthGuard&) -> ParseDepthGuard& = delete;
};

template <typename T>
auto toString(const T& value) -> std::string;

template <>
auto toString<char>(const char& value) -> std::string {
    return std::format(R"("{}")", value);
}

template <typename T>
auto fromString(std::string_view str) -> T {
    return nbn::core::utils::string::fromString<T>(str);
}

nbn_export_core auto escapeString(std::string_view value) -> std::string {
    size_t extraChars{0};
    for (const char c : value) {
        switch (c) {
            case QUOTE:
            case BACKSLASH:
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
                ++extraChars;
                break;
            default:
                break;
        }
    }

    std::string escaped;
    escaped.reserve(value.size() + extraChars);

    for (const char c : value) {
        switch (c) {
            case QUOTE:
                escaped.push_back(BACKSLASH);
                escaped.push_back(QUOTE);
                break;
            case BACKSLASH:
                escaped.push_back(BACKSLASH);
                escaped.push_back(BACKSLASH);
                break;
            case '\b':
                escaped.push_back(BACKSLASH);
                escaped.push_back('b');
                break;
            case '\f':
                escaped.push_back(BACKSLASH);
                escaped.push_back('f');
                break;
            case '\n':
                escaped.push_back(BACKSLASH);
                escaped.push_back('n');
                break;
            case '\r':
                escaped.push_back(BACKSLASH);
                escaped.push_back('r');
                break;
            case '\t':
                escaped.push_back(BACKSLASH);
                escaped.push_back('t');
                break;
            default:
                escaped.push_back(c);
                break;
        }
    }

    return escaped;
}

nbn_export_core auto validateInputSize(std::string_view json) -> void {
    nbn::log::fatal_if(json.size() > MAX_INPUT_SIZE,
                       std::format("Invalid JSON: input size {} exceeds the maximum of {} bytes.", json.size(), MAX_INPUT_SIZE));
}

auto hexValue(char c) -> uint32_t {
    if (c >= '0' && c <= '9') {
        return static_cast<uint32_t>(c - '0');
    }
    if (c >= 'a' && c <= 'f') {
        return HEX_ALPHA_OFFSET + static_cast<uint32_t>(c - 'a');
    }
    if (c >= 'A' && c <= 'F') {
        return HEX_ALPHA_OFFSET + static_cast<uint32_t>(c - 'A');
    }
    nbn::log::fatal(std::format("Invalid JSON string: Invalid unicode escape hex character '{}'.", c));
}

auto parseHex4(std::string_view json, size_t start) -> uint32_t {
    nbn::log::fatal_if(start + (UNICODE_HEX_DIGITS - 1U) >= json.size(),
                       "Invalid JSON string: Unexpected end of input in unicode escape.");

    uint32_t codepoint{0};
    for (size_t i = 0; i < UNICODE_HEX_DIGITS; ++i) {
        codepoint = (codepoint << 4U) | hexValue(json[start + i]);
    }
    return codepoint;
}

auto appendUtf8(std::string& out, uint32_t cp) -> void {
    nbn::log::fatal_if(cp > UTF8_MAX_CODEPOINT, std::format("Invalid JSON string: Invalid unicode code point U+{:X}.", cp));

    if (cp <= UTF8_MAX_1_BYTE) {
        out += static_cast<char>(cp);
    } else if (cp <= UTF8_MAX_2_BYTES) {
        out += static_cast<char>(UTF8_LEAD_2_BYTES | ((cp >> SHIFT_6) & UTF8_MASK_5_BITS));
        out += static_cast<char>(UTF8_CONTINUATION | (cp & UTF8_MASK_6_BITS));
    } else if (cp <= UTF8_MAX_3_BYTES) {
        out += static_cast<char>(UTF8_LEAD_3_BYTES | ((cp >> SHIFT_12) & UTF8_MASK_4_BITS));
        out += static_cast<char>(UTF8_CONTINUATION | ((cp >> SHIFT_6) & UTF8_MASK_6_BITS));
        out += static_cast<char>(UTF8_CONTINUATION | (cp & UTF8_MASK_6_BITS));
    } else {
        out += static_cast<char>(UTF8_LEAD_4_BYTES | ((cp >> SHIFT_18) & UTF8_MASK_3_BITS));
        out += static_cast<char>(UTF8_CONTINUATION | ((cp >> SHIFT_12) & UTF8_MASK_6_BITS));
        out += static_cast<char>(UTF8_CONTINUATION | ((cp >> SHIFT_6) & UTF8_MASK_6_BITS));
        out += static_cast<char>(UTF8_CONTINUATION | (cp & UTF8_MASK_6_BITS));
    }
}

auto unexpectedCharMessage(std::string_view json, size_t pos, char expected) -> std::string {
    if (pos >= json.size()) {
        return std::format(R"(Invalid JSON string: Expected '{}' at index {} and got end of input)", expected, pos);
    }
    return std::format(R"(Invalid JSON string: Expected '{}' at index {} and got '{}')", expected, pos, json[pos]);
}

auto isJsonWhitespace(char ch) -> bool {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

auto skipWhitespace(std::string_view json, size_t& pos) -> void {
    while (pos < json.size() && isJsonWhitespace(json[pos])) {
        ++pos;
    }
}

nbn_export_core auto validateEnd(std::string_view json, size_t pos) -> void {
    skipWhitespace(json, pos);
    nbn::log::fatal_if(pos != json.size(), std::format("Invalid JSON: trailing data at index {}.", pos));
}

auto parseString(std::string_view json, size_t& pos) -> std::string {
    std::string value;

    validateInputSize(json);
    skipWhitespace(json, pos);
    nbn::log::fatal_if(pos >= json.size(), "Invalid JSON string: Unexpected end of input.");
    nbn::log::fatal_if(json[pos] != QUOTE, unexpectedCharMessage(json, pos, QUOTE));
    ++pos;  // Skip the opening quote

    size_t fastPos = pos;
    while (fastPos < json.size() && json[fastPos] != QUOTE && json[fastPos] != BACKSLASH) {
        nbn::log::fatal_if(static_cast<unsigned char>(json[fastPos]) < JSON_CONTROL_CHARACTER_LIMIT,
                           "Invalid JSON string: unescaped control character.");
        ++fastPos;
    }
    if (fastPos < json.size() && json[fastPos] == QUOTE) {
        value.assign(json.substr(pos, fastPos - pos));
        nbn::log::fatal_if(value.size() > maximumDecodedStringSize(json),
                           "Invalid JSON string: decoded string exceeds the maximum size.");
        pos = fastPos + 1U;
        return value;
    }

    // Reserve a conservative upper bound to avoid repeated reallocations in escaped strings.
    value.reserve(json.size() - pos);

    while (pos < json.size() && json[pos] != QUOTE) {
        const size_t segmentStart = pos;
        while (pos < json.size() && json[pos] != QUOTE && json[pos] != BACKSLASH) {
            nbn::log::fatal_if(static_cast<unsigned char>(json[pos]) < JSON_CONTROL_CHARACTER_LIMIT,
                               "Invalid JSON string: unescaped control character.");
            ++pos;
        }
        if (pos > segmentStart) {
            value.append(json.substr(segmentStart, pos - segmentStart));
        }
        nbn::log::fatal_if(value.size() > maximumDecodedStringSize(json),
                           "Invalid JSON string: decoded string exceeds the maximum size.");

        if (pos >= json.size() || json[pos] == QUOTE) {
            break;
        }

        if (json[pos] == BACKSLASH) {
            ++pos;  // Skip the escape character

            nbn::log::fatal_if(pos >= json.size(), "Invalid JSON string: Unexpected end of input after escape character.");

            char escapedChar = json[pos];
            uint32_t codepoint{};
            uint32_t low{};

            switch (escapedChar) {
                case QUOTE:
                    value.push_back(QUOTE);
                    break;
                case BACKSLASH:
                    value.push_back(BACKSLASH);
                    break;
                case SLASH:
                    value.push_back(SLASH);
                    break;
                case 'b':
                    value.push_back('\b');
                    break;
                case 'f':
                    value.push_back('\f');
                    break;
                case 'n':
                    value.push_back('\n');
                    break;
                case 'r':
                    value.push_back('\r');
                    break;
                case 't':
                    value.push_back('\t');
                    break;
                case 'u':
                    codepoint = parseHex4(json, pos + 1);
                    pos += UNICODE_HEX_DIGITS;

                    if (codepoint >= HIGH_SURROGATE_START && codepoint <= HIGH_SURROGATE_END) {
                        nbn::log::fatal_if(pos + 2 >= json.size() || json[pos + 1] != BACKSLASH || json[pos + 2] != 'u',
                                           "Invalid JSON string: Missing low surrogate after high surrogate.");

                        low = parseHex4(json, pos + 3);
                        nbn::log::fatal_if(low < LOW_SURROGATE_START || low > LOW_SURROGATE_END,
                                           "Invalid JSON string: Invalid low surrogate in unicode escape.");
                        codepoint =
                            SURROGATE_BASE + (((codepoint - HIGH_SURROGATE_START) << SHIFT_10) | (low - LOW_SURROGATE_START));
                        pos += SURROGATE_SEQUENCE_TAIL_LEN;
                    } else if (codepoint >= LOW_SURROGATE_START && codepoint <= LOW_SURROGATE_END) {
                        nbn::log::fatal("Invalid JSON string: Unexpected low surrogate without preceding high surrogate.");
                    }

                    appendUtf8(value, codepoint);
                    nbn::log::fatal_if(value.size() > maximumDecodedStringSize(json),
                                       "Invalid JSON string: decoded string exceeds the maximum size.");
                    break;
                default:
                    nbn::log::fatal(std::format(R"(Invalid JSON string: Expected one of '{}' at index {} and got '{}')",
                                                R"([\"\/bfnrtu])", pos, json[pos]));
            }
        }

        ++pos;
    }

    if (pos >= json.size()) {
        nbn::log::fatal("Invalid JSON string: Unexpected end of input after string.");
    }
    ++pos;  // Move past the closing quote

    return value;
}

auto beautifyJson(std::string_view json) -> std::string {
    constexpr int indentSize = 2;

    std::string pretty{};
    pretty.reserve(json.size() + (json.size() / 4));

    int indentLevel{0};
    bool isInString{false};
    bool isEscaped{false};

    auto appendIndent = [&pretty, &indentLevel]() {
        pretty.append(static_cast<size_t>(indentLevel) * static_cast<size_t>(indentSize), ' ');
    };

    for (const char ch : json) {
        if (isEscaped) {
            pretty.push_back(ch);
            isEscaped = false;
            continue;
        }

        if (ch == BACKSLASH) {
            pretty.push_back(ch);
            isEscaped = isInString;
            continue;
        }

        if (ch == QUOTE) {
            isInString = !isInString;
            pretty.push_back(ch);
            continue;
        }

        if (isInString) {
            pretty.push_back(ch);
            continue;
        }

        switch (ch) {
            case OPEN_BRACE:
            case OPEN_BRACKET:
                pretty.push_back(ch);
                pretty.push_back('\n');
                ++indentLevel;
                appendIndent();
                break;
            case CLOSE_BRACE:
            case CLOSE_BRACKET:
                pretty.push_back('\n');
                --indentLevel;
                appendIndent();
                pretty.push_back(ch);
                break;
            case COMMA:
                pretty.push_back(ch);
                pretty.push_back('\n');
                appendIndent();
                break;
            case COLON:
                pretty.push_back(ch);
                pretty.push_back(' ');
                break;
            case ' ':
            case '\n':
            case '\r':
            case '\t':
                break;
            default:
                pretty.push_back(ch);
                break;
        }
    }

    return pretty;
}

auto deBeautifyJson(std::string_view json) -> std::string {
    std::string deBeautifiedJson{};
    deBeautifiedJson.reserve(json.size());

    bool isInString{false};
    bool isEscaped{false};

    for (const char ch : json) {
        if (isEscaped) {
            deBeautifiedJson.push_back(ch);
            isEscaped = false;
            continue;
        }

        if (ch == BACKSLASH) {
            deBeautifiedJson.push_back(ch);
            isEscaped = isInString;
            continue;
        }

        if (ch == QUOTE) {
            isInString = !isInString;
            deBeautifiedJson.push_back(ch);
            continue;
        }

        // Skip whitespace outside of strings
        if (!isInString && std::isspace(static_cast<unsigned char>(ch)) != 0) {
            continue;
        }

        deBeautifiedJson.push_back(ch);
    }

    return deBeautifiedJson;
}

auto skipValue(std::string_view json, size_t& pos) -> void;
auto checkConstant(std::string_view json, size_t& pos, std::string_view constant) -> void;

auto skipString(std::string_view json, size_t& pos) -> void {
    nbn::log::fatal_if(pos >= json.size(), "Invalid JSON string: Unexpected end of input.");
    nbn::log::fatal_if(json[pos] != QUOTE, unexpectedCharMessage(json, pos, QUOTE));
    ++pos;

    while (pos < json.size()) {
        while (pos < json.size() && json[pos] != QUOTE && json[pos] != BACKSLASH) {
            nbn::log::fatal_if(static_cast<unsigned char>(json[pos]) < JSON_CONTROL_CHARACTER_LIMIT,
                               "Invalid JSON string: unescaped control character.");
            ++pos;
        }

        if (pos >= json.size()) {
            break;
        }

        if (json[pos] == QUOTE) {
            ++pos;
            return;
        }

        ++pos;
        nbn::log::fatal_if(pos >= json.size(), "Invalid JSON string: Unexpected end of input after escape character.");

        const char escapedChar = json[pos];
        switch (escapedChar) {
            case QUOTE:
            case BACKSLASH:
            case SLASH:
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
                ++pos;
                break;
            case 'u': {
                auto codepoint{parseHex4(json, pos + 1)};
                pos += UNICODE_HEX_DIGITS + 1U;

                if (codepoint >= HIGH_SURROGATE_START && codepoint <= HIGH_SURROGATE_END) {
                    nbn::log::fatal_if(pos + 1 >= json.size() || json[pos] != BACKSLASH || json[pos + 1] != 'u',
                                       "Invalid JSON string: Missing low surrogate after high surrogate.");

                    auto low{parseHex4(json, pos + 2)};
                    nbn::log::fatal_if(low < LOW_SURROGATE_START || low > LOW_SURROGATE_END,
                                       "Invalid JSON string: Invalid low surrogate in unicode escape.");
                    pos += SURROGATE_SEQUENCE_TAIL_LEN;
                } else if (codepoint >= LOW_SURROGATE_START && codepoint <= LOW_SURROGATE_END) {
                    nbn::log::fatal("Invalid JSON string: Unexpected low surrogate without preceding high surrogate.");
                }
                break;
            }
            default:
                nbn::log::fatal(std::format(R"(Invalid JSON string: Expected one of '{}' at index {} and got '{}')",
                                            R"([\"\/bfnrtu])", pos, json[pos]));
        }
    }

    nbn::log::fatal_if(pos >= json.size(), "Invalid JSON string: Unexpected end of input after string.");
}

auto skipNumber(std::string_view json, size_t& pos) -> void {
    if (pos < json.size() && json[pos] == '-') {
        ++pos;
    }

    nbn::log::fatal_if(pos >= json.size(), "Invalid JSON number: missing integer part.");

    if (json[pos] == '0') {
        ++pos;
        if (pos < json.size() && std::isdigit(static_cast<unsigned char>(json[pos])) != 0) {
            nbn::log::fatal("Invalid JSON number: leading zeros are not allowed.");
        }
    } else if (std::isdigit(static_cast<unsigned char>(json[pos])) != 0) {
        while (pos < json.size() && std::isdigit(static_cast<unsigned char>(json[pos])) != 0) {
            ++pos;
        }
    } else {
        nbn::log::fatal("Invalid JSON number: missing integer digits.");
    }

    if (pos < json.size() && json[pos] == '.') {
        ++pos;
        nbn::log::fatal_if(pos >= json.size() || std::isdigit(static_cast<unsigned char>(json[pos])) == 0,
                           "Invalid JSON number: missing fractional digits.");
        while (pos < json.size() && std::isdigit(static_cast<unsigned char>(json[pos])) != 0) {
            ++pos;
        }
    }

    if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
        ++pos;
        if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
            ++pos;
        }
        nbn::log::fatal_if(pos >= json.size() || std::isdigit(static_cast<unsigned char>(json[pos])) == 0,
                           "Invalid JSON number: missing exponent digits.");
        while (pos < json.size() && std::isdigit(static_cast<unsigned char>(json[pos])) != 0) {
            ++pos;
        }
    }

    if (pos < json.size() && (std::isalpha(static_cast<unsigned char>(json[pos])) != 0 || json[pos] == '+' || json[pos] == '-')) {
        nbn::log::fatal(std::format("Invalid JSON number: unexpected character '{}' after number.", json[pos]));
    }
}

auto skipObject(std::string_view json, size_t& pos) -> void {
    nbn::log::fatal_if(pos >= json.size() || json[pos] != OPEN_BRACE, unexpectedCharMessage(json, pos, OPEN_BRACE));
    ++pos;
    skipWhitespace(json, pos);
    std::unordered_set<std::string> keys;

    if (pos < json.size() && json[pos] == CLOSE_BRACE) {
        ++pos;
        return;
    }

    while (pos < json.size() && json[pos] != CLOSE_BRACE) {
        skipWhitespace(json, pos);
        nbn::log::fatal_if(pos >= json.size(), unexpectedCharMessage(json, pos, CLOSE_BRACE));
        const std::string key{parseString(json, pos)};
        nbn::log::fatal_if(!keys.insert(key).second, "Invalid JSON object: duplicate key.");
        skipWhitespace(json, pos);
        nbn::log::fatal_if(pos >= json.size() || json[pos] != COLON, unexpectedCharMessage(json, pos, COLON));
        ++pos;
        skipWhitespace(json, pos);
        skipValue(json, pos);
        skipWhitespace(json, pos);

        if (pos < json.size() && json[pos] == COMMA) {
            ++pos;
            skipWhitespace(json, pos);
            nbn::log::fatal_if(pos >= json.size() || json[pos] == CLOSE_BRACE,
                               "Invalid JSON object: trailing comma is not allowed.");
        } else if (pos < json.size() && json[pos] != CLOSE_BRACE) {
            nbn::log::fatal(unexpectedCharMessage(json, pos, CLOSE_BRACE));
        }
    }

    nbn::log::fatal_if(pos >= json.size() || json[pos] != CLOSE_BRACE, unexpectedCharMessage(json, pos, CLOSE_BRACE));
    ++pos;
}

auto skipArray(std::string_view json, size_t& pos) -> void {
    nbn::log::fatal_if(pos >= json.size() || json[pos] != OPEN_BRACKET, unexpectedCharMessage(json, pos, OPEN_BRACKET));
    ++pos;
    skipWhitespace(json, pos);

    if (pos < json.size() && json[pos] == CLOSE_BRACKET) {
        ++pos;
        return;
    }

    while (pos < json.size() && json[pos] != CLOSE_BRACKET) {
        skipValue(json, pos);
        skipWhitespace(json, pos);

        if (pos < json.size() && json[pos] == COMMA) {
            ++pos;
            skipWhitespace(json, pos);
            nbn::log::fatal_if(pos >= json.size() || json[pos] == CLOSE_BRACKET,
                               "Invalid JSON array: trailing comma is not allowed.");
        } else if (pos < json.size() && json[pos] != CLOSE_BRACKET) {
            nbn::log::fatal(unexpectedCharMessage(json, pos, CLOSE_BRACKET));
        }
    }

    nbn::log::fatal_if(pos >= json.size() || json[pos] != CLOSE_BRACKET, unexpectedCharMessage(json, pos, CLOSE_BRACKET));
    ++pos;
}

auto skipValue(std::string_view json, size_t& pos) -> void {
    const ParseDepthGuard depthGuard{json};
    skipWhitespace(json, pos);
    nbn::log::fatal_if(pos >= json.size(), "Invalid JSON string: Unexpected end of input.");
    switch (json[pos]) {
        case OPEN_BRACE:
            skipObject(json, pos);
            return;
        case OPEN_BRACKET:
            skipArray(json, pos);
            return;
        case QUOTE:
            skipString(json, pos);
            return;
        case 'n':
            checkConstant(json, pos, constants::NULL_PTR);
            return;
        case 't':
            checkConstant(json, pos, constants::TRUE);
            return;
        case 'f':
            checkConstant(json, pos, constants::FALSE);
            return;
        default:
            if (std::isdigit(static_cast<unsigned char>(json[pos])) != 0 || json[pos] == '-') {
                skipNumber(json, pos);
                return;
            }
            break;
    }

    nbn::log::fatal(std::format("Invalid JSON string: Unexpected character '{}'. json: '{}', pos: {}", json[pos], json, pos));
}

auto parseValueView(std::string_view json, size_t& pos) -> std::string_view;

auto parseNumber(std::string_view json, size_t& pos) -> std::string {
    skipWhitespace(json, pos);
    size_t startPos = pos;
    skipNumber(json, pos);

    return std::string(json.substr(startPos, pos - startPos));
}

template <typename T>
auto deserializeNumber(std::string_view str) -> T {
    validateInputSize(str);
    size_t pos{0};
    const auto token{parseNumber(str, pos)};
    validateEnd(str, pos);
    T value{};
    const auto tokenBegin = token.data();
    const auto tokenEnd = std::next(tokenBegin, static_cast<std::ptrdiff_t>(token.size()));
    const auto [end, error] = std::from_chars(tokenBegin, tokenEnd, value);
    nbn::log::fatal_if(error != std::errc{} || end != tokenEnd,
                       std::format("Invalid JSON number or value out of range: '{}'.", token));
    return value;
}

auto parseObject(std::unordered_map<std::string, std::string>& propertiesAsString, std::string_view json, size_t& pos) -> void {
    validateInputSize(json);
    propertiesAsString.clear();
    propertiesAsString.reserve(INITIAL_OBJECT_BUCKETS);
    skipWhitespace(json, pos);
    nbn::log::fatal_if(pos >= json.size() || json[pos] != OPEN_BRACE, unexpectedCharMessage(json, pos, OPEN_BRACE));
    ++pos;  // Skip the opening brace
    skipWhitespace(json, pos);

    if (pos < json.size() && json[pos] == CLOSE_BRACE) {
        ++pos;
        return;
    }

    while (pos < json.size() && json[pos] != CLOSE_BRACE) {
        skipWhitespace(json, pos);
        nbn::log::fatal_if(pos >= json.size(), unexpectedCharMessage(json, pos, CLOSE_BRACE));
        if (json[pos] != QUOTE) {
            nbn::log::fatal(unexpectedCharMessage(json, pos, QUOTE));
        }

        auto key{parseString(json, pos)};
        skipWhitespace(json, pos);
        nbn::log::fatal_if(pos >= json.size() || json[pos] != COLON, unexpectedCharMessage(json, pos, COLON));
        ++pos;  // Skip the closing colon of the key
        skipWhitespace(json, pos);

        enforceCollectionLimit(json, propertiesAsString.size() + 1U, "object property");
        const auto [_, inserted] = propertiesAsString.emplace(key, std::string(parseValueView(json, pos)));
        nbn::log::fatal_if(!inserted, std::format("Invalid JSON object: duplicate key '{}'.", key));
        skipWhitespace(json, pos);

        if (pos < json.size() && json[pos] == COMMA) {
            ++pos;  // Skip the comma
            skipWhitespace(json, pos);
            nbn::log::fatal_if(pos >= json.size() || json[pos] == CLOSE_BRACE,
                               "Invalid JSON object: trailing comma is not allowed.");
        } else if (pos < json.size() && json[pos] != CLOSE_BRACE) {
            nbn::log::fatal(unexpectedCharMessage(json, pos, CLOSE_BRACE));
        }
    }

    nbn::log::fatal_if(pos >= json.size() || json[pos] != CLOSE_BRACE, unexpectedCharMessage(json, pos, CLOSE_BRACE));
    ++pos;  // Skip the closing brace
}

auto parseObject(std::string_view json, size_t& pos) -> std::string {
    validateInputSize(json);
    skipWhitespace(json, pos);
    nbn::log::fatal_if(pos >= json.size() || json[pos] != OPEN_BRACE, unexpectedCharMessage(json, pos, OPEN_BRACE));
    ++pos;

    std::string result{"{"};
    result.reserve(json.size());
    skipWhitespace(json, pos);

    if (pos < json.size() && json[pos] == CLOSE_BRACE) {
        ++pos;
        return result + "}";
    }

    bool isFirst{true};
    while (pos < json.size() && json[pos] != CLOSE_BRACE) {
        skipWhitespace(json, pos);
        nbn::log::fatal_if(pos >= json.size(), unexpectedCharMessage(json, pos, CLOSE_BRACE));
        if (json[pos] != QUOTE) {
            nbn::log::fatal(unexpectedCharMessage(json, pos, QUOTE));
        }

        auto key{parseString(json, pos)};
        skipWhitespace(json, pos);
        nbn::log::fatal_if(pos >= json.size() || json[pos] != COLON, unexpectedCharMessage(json, pos, COLON));
        ++pos;  // Skip the closing colon of the key
        skipWhitespace(json, pos);
        if (!isFirst) {
            result.push_back(COMMA);
        }
        result.push_back(QUOTE);
        const auto escapedKey = escapeString(key);
        result.append(escapedKey);
        result.append(R"(":)");
        const auto value = parseValueView(json, pos);
        result.append(value);
        isFirst = false;
        skipWhitespace(json, pos);

        if (pos < json.size() && json[pos] == COMMA) {
            ++pos;
            skipWhitespace(json, pos);
            nbn::log::fatal_if(pos >= json.size() || json[pos] == CLOSE_BRACE,
                               "Invalid JSON object: trailing comma is not allowed.");
        } else if (pos < json.size() && json[pos] != CLOSE_BRACE) {
            nbn::log::fatal(unexpectedCharMessage(json, pos, CLOSE_BRACE));
        }
    }
    nbn::log::fatal_if(pos >= json.size() || json[pos] != CLOSE_BRACE, unexpectedCharMessage(json, pos, CLOSE_BRACE));
    ++pos;  // Skip the closing brace
    result += "}";
    return result;
}

auto checkConstant(std::string_view json, size_t& pos, std::string_view constant) -> void {
    auto length{constant.size()};
    if (json.size() < pos + length) {
        nbn::log::fatal(std::format(R"(Invalid JSON string: Unexpected end of input parsing constant '{}')", constant));
    }
    nbn::log::fatal_if(
        json.substr(pos, length) != constant,
        std::format(R"(Invalid JSON string: Expected '{}' at index {} and got '{}')", constant, pos, json.substr(pos, length)));
    pos += length;
}

auto parseValueView(std::string_view json, size_t& pos) -> std::string_view {
    validateInputSize(json);
    skipWhitespace(json, pos);
    const size_t valueStart = pos;
    skipValue(json, pos);
    return json.substr(valueStart, pos - valueStart);
}

}  // namespace json

nbn_export_core auto deBeautifyJson(std::string_view json) -> std::string {
    return nbn::core::serialization::json::deBeautifyJson(json);
}

namespace value {

// --- serialize ---
template <>
auto serialize<char>(const char& value) -> std::string {
    return json::toString(value);
}

template <>
auto serialize<unsigned char>(const unsigned char& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<short>(const short& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<unsigned short>(const unsigned short& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<int>(const int& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<unsigned int>(const unsigned int& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<long>(const long& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<unsigned long>(const unsigned long& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<long long>(const long long& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<unsigned long long>(const unsigned long long& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<float>(const float& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<double>(const double& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<long double>(const long double& value) -> std::string {
    return nbn::core::utils::string::toString(value);
}

template <>
auto serialize<bool>(const bool& value) -> std::string {
    return value ? nbn::core::serialization::json::constants::TRUE : nbn::core::serialization::json::constants::FALSE;
}

template <>
auto serialize<std::nullptr_t>([[maybe_unused]] const std::nullptr_t& value) -> std::string {
    return nbn::core::serialization::json::constants::NULL_PTR;
}

template <>
auto serialize<std::string>(const std::string& value) -> std::string {
    auto escaped{json::escapeString(value)};
    escaped.insert(escaped.begin(), json::QUOTE);
    escaped.push_back(json::QUOTE);
    return escaped;
}

template <>
auto serialize<std::chrono::nanoseconds>(const std::chrono::nanoseconds& value) -> std::string {
    return nbn::core::utils::string::toString(value.count());
}

template <>
auto serialize<serializable_ptr_t>(const serializable_ptr_t& value) -> std::string {
    return object::serialize(value.get());
}

template <>
auto serialize<serializable_vector_t>(const serializable_vector_t& value) -> std::string {
    return array::serialize(value);
}

template <>
auto serialize<serializable_variants_t>(const serializable_variants_t& value) -> std::string {
    // GCC emits one source-mapped visitor function per variant alternative. The nanoseconds
    // visitor is instantiated in client translation units and its duplicate counter is not
    // attributed to this production definition, although the duration path is exercised.
    return std::visit([](const auto& value) { return serialize(value); }, value);
}

// --- deserialize ---

template <>
auto deserialize<char>(std::string_view str) -> char {
    size_t pos{0};
    const auto value{nbn::core::serialization::json::parseString(str, pos)};
    nbn::core::serialization::json::validateEnd(str, pos);
    nbn::log::fatal_if(value.size() != 1, std::format("Cannot convert string '{}' to char", str));
    return value.front();
}

template <>
auto deserialize<unsigned char>(std::string_view str) -> unsigned char {
    return nbn::core::serialization::json::deserializeNumber<unsigned char>(str);
}

template <>
auto deserialize<short>(std::string_view str) -> short {
    return nbn::core::serialization::json::deserializeNumber<short>(str);
}

template <>
auto deserialize<unsigned short>(std::string_view str) -> unsigned short {
    return nbn::core::serialization::json::deserializeNumber<unsigned short>(str);
}

template <>
auto deserialize<int>(std::string_view str) -> int {
    return nbn::core::serialization::json::deserializeNumber<int>(str);
}

template <>
auto deserialize<unsigned int>(std::string_view str) -> unsigned int {
    return nbn::core::serialization::json::deserializeNumber<unsigned int>(str);
}

template <>
auto deserialize<long>(std::string_view str) -> long {
    return nbn::core::serialization::json::deserializeNumber<long>(str);
}

template <>
auto deserialize<unsigned long>(std::string_view str) -> unsigned long {
    return nbn::core::serialization::json::deserializeNumber<unsigned long>(str);
}

template <>
auto deserialize<long long>(std::string_view str) -> long long {
    return nbn::core::serialization::json::deserializeNumber<long long>(str);
}

template <>
auto deserialize<unsigned long long>(std::string_view str) -> unsigned long long {
    return nbn::core::serialization::json::deserializeNumber<unsigned long long>(str);
}

template <>
auto deserialize<float>(std::string_view str) -> float {
    return nbn::core::serialization::json::deserializeNumber<float>(str);
}

template <>
auto deserialize<double>(std::string_view str) -> double {
    return nbn::core::serialization::json::deserializeNumber<double>(str);
}

template <>
auto deserialize<long double>(std::string_view str) -> long double {
    return nbn::core::serialization::json::deserializeNumber<long double>(str);
}

template <>
auto deserialize<bool>(std::string_view str) -> bool {
    constexpr size_t TRUE_LITERAL_LENGTH = 4U;
    constexpr size_t FALSE_LITERAL_LENGTH = 5U;
    size_t pos{0};
    nbn::core::serialization::json::skipWhitespace(str, pos);
    const bool value{str.substr(pos, 4) == nbn::core::serialization::json::constants::TRUE};
    const bool isFalse{str.substr(pos, 5) == nbn::core::serialization::json::constants::FALSE};
    nbn::log::fatal_if(!value && !isFalse, std::format("Cannot convert string '{}' to bool", str));
    pos += value ? TRUE_LITERAL_LENGTH : FALSE_LITERAL_LENGTH;
    nbn::core::serialization::json::validateEnd(str, pos);
    return value;
}

template <>
auto deserialize<std::nullptr_t>(std::string_view str) -> std::nullptr_t {
    size_t pos{0};
    nbn::core::serialization::json::skipWhitespace(str, pos);
    nbn::log::fatal_if(str.substr(pos, 4) != nbn::core::serialization::json::constants::NULL_PTR,
                       std::format("Cannot convert string '{}' to nullptr", str));
    pos += 4U;
    nbn::core::serialization::json::validateEnd(str, pos);
    return nullptr;
}

template <>
auto deserialize<std::chrono::nanoseconds>(std::string_view str) -> std::chrono::nanoseconds {
    return std::chrono::nanoseconds{nbn::core::serialization::json::deserializeNumber<long long>(str)};
}

template <>
auto deserialize<std::string>(std::string_view str) -> std::string {
    size_t pos = 0;
    auto result = nbn::core::serialization::json::parseString(str, pos);
    nbn::core::serialization::json::validateEnd(str, pos);
    return result;
}

template <>
auto deserialize<serializable_ptr_t>(std::string_view str) -> serializable_ptr_t {
    return object::deserialize(str);
}

template <>
auto deserialize<serializable_vector_t>(std::string_view str) -> serializable_vector_t {
    return array::deserialize<serializable_vector_t>(str);
}

namespace arithmetic_deserializers {

using deserializer = std::function<serializable_variants_t(std::string_view)>;

template <typename T>
auto findDeserializerBySize(const T& deserializers, size_t size) -> deserializer {
    for (const auto& [maximumSize, candidate] : deserializers) {
        if (size <= maximumSize) {
            return candidate;
        }
    }
    return {};
}

namespace integer {

auto parseUnsignedChar(std::string_view str) -> serializable_variants_t {
    unsigned char value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec == std::errc{} && ptr == str.data() + str.size()) {
        return value;
    }
    return nbn::core::utils::string::fromString<unsigned char>(str);
}

auto parseUnsignedShort(std::string_view str) -> serializable_variants_t {
    unsigned short value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec == std::errc{} && ptr == str.data() + str.size()) {
        return value;
    }
    return nbn::core::utils::string::fromString<unsigned short>(str);
}

auto parseUnsignedInt(std::string_view str) -> serializable_variants_t {
    unsigned int value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec == std::errc{} && ptr == str.data() + str.size()) {
        return value;
    }
    return nbn::core::utils::string::fromString<unsigned int>(str);
}

auto parseUnsignedLong(std::string_view str) -> serializable_variants_t {
    unsigned long value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec == std::errc{} && ptr == str.data() + str.size()) {
        return value;
    }
    return nbn::core::utils::string::fromString<unsigned long>(str);
}

auto parseUnsignedLongLong(std::string_view str) -> serializable_variants_t {
    unsigned long long value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec == std::errc{} && ptr == str.data() + str.size()) {
        return value;
    }
    return nbn::core::utils::string::fromString<unsigned long long>(str);
}

auto parseChar(std::string_view str) -> serializable_variants_t {
    int value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec == std::errc{} && ptr == str.data() + str.size()) {
        return static_cast<char>(value);
    }
    return nbn::core::utils::string::fromString<char>(str);
}

auto parseShort(std::string_view str) -> serializable_variants_t {
    short value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec == std::errc{} && ptr == str.data() + str.size()) {
        return value;
    }
    return nbn::core::utils::string::fromString<short>(str);
}

auto parseInt(std::string_view str) -> serializable_variants_t {
    int value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec == std::errc{} && ptr == str.data() + str.size()) {
        return value;
    }
    return nbn::core::utils::string::fromString<int>(str);
}

auto parseLong(std::string_view str) -> serializable_variants_t {
    long value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec == std::errc{} && ptr == str.data() + str.size()) {
        return value;
    }
    return nbn::core::utils::string::fromString<long>(str);
}

auto parseLongLong(std::string_view str) -> serializable_variants_t {
    long long value{};
    const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
    if (ec == std::errc{} && ptr == str.data() + str.size()) {
        return value;
    }
    return nbn::core::utils::string::fromString<long long>(str);
}

using namespace nbn::core::utils::string;
static const std::array<std::pair<size_t, deserializer>, 5U> typeMaxLength{
    {{toString(std::numeric_limits<unsigned char>::max()).size(), parseUnsignedChar},
     {toString(std::numeric_limits<unsigned short>::max()).size(), parseUnsignedShort},
     {toString(std::numeric_limits<unsigned int>::max()).size(), parseUnsignedInt},
     {toString(std::numeric_limits<unsigned long>::max()).size(), parseUnsignedLong},
     {toString(std::numeric_limits<unsigned long long>::max()).size(), parseUnsignedLongLong}}};

static const std::array<std::pair<size_t, deserializer>, 5U> typeMinLength{
    {{toString(std::numeric_limits<char>::min()).size(), parseChar},
     {toString(std::numeric_limits<short>::min()).size(), parseShort},
     {toString(std::numeric_limits<int>::min()).size(), parseInt},
     {toString(std::numeric_limits<long>::min()).size(), parseLong},
     {toString(std::numeric_limits<long long>::min()).size(), parseLongLong}}};

}  // namespace integer

namespace floating_point {

using namespace nbn::core::utils::string;
static const std::array<std::pair<size_t, deserializer>, 3U> typeMaxLength{
    {{toString(std::numeric_limits<float>::max()).size(), fromString<float>},
     {toString(std::numeric_limits<double>::max()).size(), fromString<double>},
     {toString(std::numeric_limits<long double>::max()).size(), fromString<long double>}}};

static const std::array<std::pair<size_t, deserializer>, 3U> typeMinLength{
    {{toString(-std::numeric_limits<float>::max()).size(), fromString<float>},
     {toString(-std::numeric_limits<double>::max()).size(), fromString<double>},
     {toString(-std::numeric_limits<long double>::max()).size(), fromString<long double>}}};

}  // namespace floating_point

auto deserialize(std::string_view numStr) -> serializable_variants_t {
    auto isFloatingPoint{
        (numStr.find('.') != std::string::npos || numStr.find('e') != std::string::npos || numStr.find('E') != std::string::npos)};
    if (numStr[0] == '-') {
        if (isFloatingPoint) {
            if (auto selected = findDeserializerBySize(floating_point::typeMinLength, numStr.size())) {
                return selected(numStr);
            }
        } else if (auto selected = findDeserializerBySize(integer::typeMinLength, numStr.size())) {
            return selected(numStr);
        }
    } else {
        if (isFloatingPoint) {
            if (auto selected = findDeserializerBySize(floating_point::typeMaxLength, numStr.size())) {
                return selected(numStr);
            }
        } else if (auto selected = findDeserializerBySize(integer::typeMaxLength, numStr.size())) {
            return selected(numStr);
        }
    }

    nbn::log::fatal(std::format("Integer value out of range: '{}'.", numStr));
}
}  // namespace arithmetic_deserializers

template <>
auto deserialize<serializable_variants_t>(std::string_view str) -> serializable_variants_t {
    json::validateInputSize(str);
    if (str.empty()) {
        nbn::log::fatal("Cannot deserialize an empty string");
    }

    size_t pos{0};
    json::skipWhitespace(str, pos);
    nbn::log::fatal_if(pos >= str.size(), "Cannot deserialize an empty JSON value");

    // Determine the type of value represented by the string and deserialize accordingly.
    std::string stringValue{};
    switch (str[pos]) {
        case json::QUOTE:
            stringValue = json::parseString(str, pos);
            json::validateEnd(str, pos);
            return stringValue;
        case 't':
        case 'f': {
            bool isValue = (str[pos] == 't');
            json::checkConstant(str, pos, isValue ? json::constants::TRUE : json::constants::FALSE);
            json::validateEnd(str, pos);
            return isValue;
        }
        case 'n':
            json::checkConstant(str, pos, json::constants::NULL_PTR);
            json::validateEnd(str, pos);
            return nullptr;

        case nbn::core::serialization::json::OPEN_BRACKET:
            return deserialize<serializable_vector_t>(str);

        case nbn::core::serialization::json::OPEN_BRACE:
            return deserialize<serializable_ptr_t>(str);

        default:
            if (isdigit(str[pos]) || str[pos] == '-') {
                std::string numStr = json::parseNumber(str, pos);
                json::validateEnd(str, pos);
                return arithmetic_deserializers::deserialize(numStr);
            }
    }
    nbn::log::fatal(std::format("Invalid JSON string: Unexpected character '{}'.", str[pos]));
}

}  // namespace value

namespace array {

template <typename T>
auto appendSerializedValue(std::string& output, const T& value) -> void;

template <typename T>
auto appendSerializedArray(std::string& output, const T& value) -> void {
    output.push_back(nbn::core::serialization::json::OPEN_BRACKET);
    bool first{true};
    for (const auto& element : value) {
        if (!first) {
            output.push_back(nbn::core::serialization::json::COMMA);
        }
        first = false;
        appendSerializedValue(output, element);
    }
    output.push_back(nbn::core::serialization::json::CLOSE_BRACKET);
}

template <typename T>
auto appendSerializedValue(std::string& output, const T& value) -> void {
    using value_type = std::remove_cvref_t<T>;
    if constexpr (is_serializable_vector<value_type>::value) {
        appendSerializedArray(output, value);
    } else if constexpr (std::is_same_v<value_type, serializable_variants_t>) {
        std::visit([&output](const auto& alternative) { appendSerializedValue(output, alternative); }, value);
    } else if constexpr (std::is_same_v<value_type, std::string>) {
        output.push_back(nbn::core::serialization::json::QUOTE);
        output.append(nbn::core::serialization::json::escapeString(value));
        output.push_back(nbn::core::serialization::json::QUOTE);
    } else if constexpr (std::is_same_v<value_type, char>) {
        output.push_back(nbn::core::serialization::json::QUOTE);
        output.push_back(value);
        output.push_back(nbn::core::serialization::json::QUOTE);
    } else if constexpr (std::is_same_v<value_type, bool>) {
        output.append(value ? json::constants::TRUE : json::constants::FALSE);
    } else if constexpr (std::is_same_v<value_type, std::nullptr_t>) {
        output.append(json::constants::NULL_PTR);
    } else if constexpr (std::is_same_v<value_type, std::chrono::nanoseconds>) {
        output.append(nbn::core::utils::string::toString(value.count()));
    } else if constexpr (std::is_arithmetic_v<value_type>) {
        output.append(nbn::core::utils::string::toString(value));
    } else if constexpr (enum_serializable_c<value_type>) {
        output.push_back(nbn::core::serialization::json::QUOTE);
        output.append(json::escapeString(enum_serialization<value_type>::toString(value)));
        output.push_back(nbn::core::serialization::json::QUOTE);
    } else if constexpr (is_serializable_ptr<value_type>::value) {
        output.append(object::serialize(value.get()));
    } else if constexpr (std::is_base_of_v<interfaces::ISerializable, value_type>) {
        output.append(object::serialize(const_cast<value_type*>(&value)));
    } else {
        output.append(value::serialize(value));
    }
}

template <typename T>
auto serialize(const T& value) -> std::string {
    std::string output{};
    output.reserve(value.size() + 2U);
    appendSerializedArray(output, value);
    return output;
}

template <typename T>
auto deserialize(std::string_view str) -> T {
    nbn::core::serialization::json::validateInputSize(str);
    T vec;
    using value_type = nbn::core::vector_value_type_t<T>;
    if (str.empty() || str.front() != nbn::core::serialization::json::OPEN_BRACKET) {
        nbn::log::fatal(std::format("Invalid array string: missing opening bracket at index {}", 0));
    }

    if constexpr (std::is_same_v<value_type, std::string>) {
        std::string_view str_view{str};
        str_view.remove_prefix(1);
        if (!str_view.empty() && str_view.front() == nbn::core::serialization::json::CLOSE_BRACKET) {
            nbn::core::serialization::json::validateEnd(str_view, 1U);
            return vec;
        }
        for (;;) {
            size_t pos{0};
            vec.emplace_back(nbn::core::serialization::json::parseString(str_view, pos));
            nbn::core::serialization::json::enforceCollectionLimit(str, vec.size(), "array element");
            nbn::log::fatal_if(pos >= str_view.size(), "Invalid array string: missing closing bracket");
            if (str_view[pos] == nbn::core::serialization::json::CLOSE_BRACKET) {
                nbn::core::serialization::json::validateEnd(str_view, pos + 1U);
                break;
            }
            if (str_view[pos] != nbn::core::serialization::json::COMMA) {
                nbn::log::fatal(std::format("Invalid array string: missing comma at index {}", pos));
            }
            str_view.remove_prefix(pos + 1);
            if (str_view.empty()) {
                nbn::log::fatal("Invalid array string: missing closing bracket");
            }
        }
        return vec;
    } else {
        size_t pos{1};
        nbn::core::serialization::json::skipWhitespace(str, pos);

        if (pos < str.size() && str[pos] == nbn::core::serialization::json::CLOSE_BRACKET) {
            ++pos;
            nbn::core::serialization::json::validateEnd(str, pos);
            return vec;
        }

        while (pos < str.size() && str[pos] != nbn::core::serialization::json::CLOSE_BRACKET) {
            const auto value_str = nbn::core::serialization::json::parseValueView(str, pos);
            nbn::core::serialization::json::enforceCollectionLimit(str, vec.size() + 1U, "array element");
            vec.emplace_back(serialization::value::deserialize<value_type>(value_str));
            nbn::core::serialization::json::skipWhitespace(str, pos);

            if (pos < str.size() && str[pos] == nbn::core::serialization::json::COMMA) {
                ++pos;
                nbn::core::serialization::json::skipWhitespace(str, pos);
                nbn::log::fatal_if(pos >= str.size() || str[pos] == nbn::core::serialization::json::CLOSE_BRACKET,
                                   "Invalid JSON array: trailing comma is not allowed.");
            } else if (pos < str.size() && str[pos] != nbn::core::serialization::json::CLOSE_BRACKET) {
                nbn::log::fatal(
                    nbn::core::serialization::json::unexpectedCharMessage(str, pos, nbn::core::serialization::json::CLOSE_BRACKET));
            }
        }

        nbn::log::fatal_if(
            pos >= str.size() || str[pos] != nbn::core::serialization::json::CLOSE_BRACKET,
            nbn::core::serialization::json::unexpectedCharMessage(str, pos, nbn::core::serialization::json::CLOSE_BRACKET));
        ++pos;
        nbn::core::serialization::json::validateEnd(str, pos);
        return vec;
    }
}

// Explicit template instantiation for all supported types
template auto serialize<std::vector<char>>(const std::vector<char>& value) -> std::string;
template auto serialize<std::vector<unsigned char>>(const std::vector<unsigned char>& value) -> std::string;
template auto serialize<std::vector<short>>(const std::vector<short>& value) -> std::string;
template auto serialize<std::vector<unsigned short>>(const std::vector<unsigned short>& value) -> std::string;
template auto serialize<std::vector<int>>(const std::vector<int>& value) -> std::string;
template auto serialize<std::vector<unsigned int>>(const std::vector<unsigned int>& value) -> std::string;
template auto serialize<std::vector<long>>(const std::vector<long>& value) -> std::string;
template auto serialize<std::vector<unsigned long>>(const std::vector<unsigned long>& value) -> std::string;
template auto serialize<std::vector<long long>>(const std::vector<long long>& value) -> std::string;
template auto serialize<std::vector<unsigned long long>>(const std::vector<unsigned long long>& value) -> std::string;
template auto serialize<std::vector<float>>(const std::vector<float>& value) -> std::string;
template auto serialize<std::vector<double>>(const std::vector<double>& value) -> std::string;
template auto serialize<std::vector<long double>>(const std::vector<long double>& value) -> std::string;
template auto serialize<std::vector<bool>>(const std::vector<bool>& value) -> std::string;
template auto serialize<std::vector<std::nullptr_t>>(const std::vector<std::nullptr_t>& value) -> std::string;
template auto serialize<std::vector<std::string>>(const std::vector<std::string>& value) -> std::string;
template auto serialize<std::vector<std::chrono::nanoseconds>>(const std::vector<std::chrono::nanoseconds>& value) -> std::string;
template auto serialize<std::vector<serializable_ptr_t>>(const std::vector<serializable_ptr_t>& value) -> std::string;
template auto serialize<std::vector<serializable_vector_t>>(const std::vector<serializable_vector_t>& value) -> std::string;

template auto deserialize<std::vector<char>>(std::string_view str) -> std::vector<char>;
template auto deserialize<std::vector<unsigned char>>(std::string_view str) -> std::vector<unsigned char>;
template auto deserialize<std::vector<short>>(std::string_view str) -> std::vector<short>;
template auto deserialize<std::vector<unsigned short>>(std::string_view str) -> std::vector<unsigned short>;
template auto deserialize<std::vector<int>>(std::string_view str) -> std::vector<int>;
template auto deserialize<std::vector<unsigned int>>(std::string_view str) -> std::vector<unsigned int>;
template auto deserialize<std::vector<long>>(std::string_view str) -> std::vector<long>;
template auto deserialize<std::vector<unsigned long>>(std::string_view str) -> std::vector<unsigned long>;
template auto deserialize<std::vector<long long>>(std::string_view str) -> std::vector<long long>;
template auto deserialize<std::vector<unsigned long long>>(std::string_view str) -> std::vector<unsigned long long>;
template auto deserialize<std::vector<float>>(std::string_view str) -> std::vector<float>;
template auto deserialize<std::vector<double>>(std::string_view str) -> std::vector<double>;
template auto deserialize<std::vector<long double>>(std::string_view str) -> std::vector<long double>;
template auto deserialize<std::vector<bool>>(std::string_view str) -> std::vector<bool>;
template auto deserialize<std::vector<std::nullptr_t>>(std::string_view str) -> std::vector<std::nullptr_t>;
template auto deserialize<std::vector<std::string>>(std::string_view str) -> std::vector<std::string>;
template auto deserialize<std::vector<std::chrono::nanoseconds>>(std::string_view str) -> std::vector<std::chrono::nanoseconds>;
template auto deserialize<std::vector<serializable_ptr_t>>(std::string_view str) -> std::vector<serializable_ptr_t>;
template auto deserialize<std::vector<serializable_vector_t>>(std::string_view str) -> std::vector<serializable_vector_t>;

}  // namespace array

namespace object {

auto serialize(interfaces::ISerializable* value) -> std::string {
    if (value == nullptr) {
        return nbn::core::serialization::json::constants::NULL_PTR;
    }
    return value->serialize();
}

auto deserialize(std::string_view str) -> serializable_ptr_t {
    nbn::log::fatal_if(str.empty(), "Cannot deserialize an empty string");

    switch (str[0]) {
        case 'n':
            json::validateEnd(str, 4U);
            return nullptr;
        default:
            break;
    }

    size_t pos{0};
    std::string objectName{};
    std::string objectPayload{};
    std::unordered_set<std::string> rootKeys{};

    json::validateInputSize(str);
    json::skipWhitespace(str, pos);
    nbn::log::fatal_if(pos >= str.size() || str[pos] != json::OPEN_BRACE, json::unexpectedCharMessage(str, pos, json::OPEN_BRACE));
    ++pos;
    json::skipWhitespace(str, pos);
    while (pos < str.size() && str[pos] != json::CLOSE_BRACE) {
        nbn::log::fatal_if(str[pos] != json::QUOTE, json::unexpectedCharMessage(str, pos, json::QUOTE));
        auto key = json::parseString(str, pos);
        const auto [_, inserted] = rootKeys.insert(key);
        nbn::log::fatal_if(!inserted, std::format("Invalid JSON object: duplicate key '{}'.", key));
        json::skipWhitespace(str, pos);
        nbn::log::fatal_if(pos >= str.size() || str[pos] != json::COLON, json::unexpectedCharMessage(str, pos, json::COLON));
        ++pos;
        json::skipWhitespace(str, pos);
        const auto payload = json::parseValueView(str, pos);

        if (key == "nbnProject" || key == "version" || key == "nbnApiVersion") {
            // Envelope metadata is validated and consumed, but never retained as object payload.
        } else {
            nbn::log::fatal_if(!objectName.empty(), "Invalid JSON object: multiple serialized objects found at root.");
            objectName = std::move(key);
            objectPayload = std::string{payload};
        }

        json::skipWhitespace(str, pos);
        if (pos < str.size() && str[pos] == json::COMMA) {
            ++pos;
            json::skipWhitespace(str, pos);
            nbn::log::fatal_if(pos >= str.size() || str[pos] == json::CLOSE_BRACE,
                               "Invalid JSON object: trailing comma is not allowed.");
        } else if (pos < str.size() && str[pos] != json::CLOSE_BRACE) {
            nbn::log::fatal(json::unexpectedCharMessage(str, pos, json::CLOSE_BRACE));
        }
    }

    nbn::log::fatal_if(pos >= str.size() || str[pos] != json::CLOSE_BRACE,
                       json::unexpectedCharMessage(str, pos, json::CLOSE_BRACE));
    ++pos;
    json::validateEnd(str, pos);

    nbn::log::fatal_if(objectName.empty(), "Invalid JSON object: missing serialized object at root.");

    auto spObject{Object::factoryCreate(objectName)};
    spObject->deserialize(objectPayload);
    return spObject;
}

}  // namespace object

}  // namespace nbn::core::serialization