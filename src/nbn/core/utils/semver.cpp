#include <algorithm>
#include <cctype>
#include <limits>
#include <vector>

#include <nbn/core/utils/semver.h>

namespace nbn::core::utils::semver {

namespace detail {

constexpr int kLessComparison = -1;
constexpr int kEqualComparison = 0;
constexpr int kGreaterComparison = 1;
constexpr int kMissingComponent = 0;

inline auto parse(std::string_view version) -> std::optional<std::vector<int>> {
    if (version.empty()) {
        return std::nullopt;
    }

    constexpr long long base10 = 10;
    std::vector<int> components{};

    size_t pos = 0;
    while (pos < version.size()) {
        size_t end = pos;
        while (end < version.size() && version[end] != '.') {
            ++end;
        }

        const auto token = version.substr(pos, end - pos);
        if (token.empty()) {
            return std::nullopt;
        }

        long long value = 0;
        for (const auto ch : token) {
            if (!std::isdigit(static_cast<unsigned char>(ch))) {
                return std::nullopt;
            }
            value = (value * base10) + static_cast<long long>(ch - '0');
            if (value > static_cast<long long>(std::numeric_limits<int>::max())) {
                return std::nullopt;
            }
        }

        components.emplace_back(static_cast<int>(value));

        if (end == version.size()) {
            break;
        }
        pos = end + 1;
    }

    return components;
}

}  // namespace detail

auto compare(std::string_view lhs, std::string_view rhs) -> std::optional<int> {
    const auto lhsParts = detail::parse(lhs);
    const auto rhsParts = detail::parse(rhs);
    if (!lhsParts.has_value()) {
        return std::nullopt;
    }
    if (!rhsParts.has_value()) {
        return std::nullopt;
    }

    const auto& lhsValues = *lhsParts;
    const auto& rhsValues = *rhsParts;
    const auto maxParts = std::max(lhsValues.size(), rhsValues.size());
    // A valid semantic version always has at least one component; the uncovered zero-iteration mapping is compiler-generated.
    for (size_t i = 0; i < maxParts; ++i) {  // LLVM-COV EXCL_LINE
        const auto lhsValue = (i < lhsValues.size()) ? lhsValues[i] : detail::kMissingComponent;
        const auto rhsValue = (i < rhsValues.size()) ? rhsValues[i] : detail::kMissingComponent;
        if (lhsValue < rhsValue) {
            return detail::kLessComparison;
        }
        if (lhsValue > rhsValue) {
            return detail::kGreaterComparison;
        }
    }

    return detail::kEqualComparison;
}

auto isValid(std::string_view version) -> bool {
    return detail::parse(version).has_value();
}

auto isLess(std::string_view lhs, std::string_view rhs) -> bool {
    const auto result = compare(lhs, rhs);
    return result.has_value() && (*result < detail::kEqualComparison);
}

auto isLessOrEqual(std::string_view lhs, std::string_view rhs) -> bool {
    const auto result = compare(lhs, rhs);
    return result.has_value() && (*result <= detail::kEqualComparison);
}

auto isEqual(std::string_view lhs, std::string_view rhs) -> bool {
    const auto result = compare(lhs, rhs);
    return result.has_value() && (*result == detail::kEqualComparison);
}

auto isGreater(std::string_view lhs, std::string_view rhs) -> bool {
    const auto result = compare(lhs, rhs);
    return result.has_value() && (*result > detail::kEqualComparison);
}

auto isGreaterOrEqual(std::string_view lhs, std::string_view rhs) -> bool {
    const auto result = compare(lhs, rhs);
    return result.has_value() && (*result >= detail::kEqualComparison);
}

}  // namespace nbn::core::utils::semver
