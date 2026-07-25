#pragma once

#include "Object.h"

namespace nbn::core {

/**
 * @file CommandLineParser.h
 * @brief Fluent command-line argument and option parser.
 */

/**
 * @class CommandLineParser
 * @brief Parses arguments/options and dispatches callbacks.
 *
 * @par Usage example
 * @code{.cpp}
 * nbn::core::CommandLineParser parser(args, "Example app");
 *
 * parser.addOption("--verbose", "Enable verbose logging", [] {
 *           nbn::log::info("verbose mode enabled");
 *       })
 *     .addArgument("input", true, "Input file", [](std::string_view value) {
 *           nbn::log::info(std::format("input: {}", value));
 *       });
 *
 * return parser.parse();
 * @endcode
 */

class nbn_export_core CommandLineParser : public Object {
   public:
    using action_t = std::function<void(void)>;
    using action_with_arguments_t = std::function<void(std::string_view)>;

    constexpr static size_t default_help_max_columns = 80;

    explicit CommandLineParser(const std::vector<std::string>& args,
                               std::string_view help,
                               size_t helpMaxColumns = default_help_max_columns);

    ~CommandLineParser() override;

    auto printHelp() const -> void;

    [[nodiscard]] auto parse() -> int;

    auto addArgument(std::string_view name, bool isMandatory, std::string_view help, action_with_arguments_t action)
        -> CommandLineParser&;

    auto addAnyArgumentsSuffix(action_with_arguments_t action) -> CommandLineParser&;

    auto addOption(std::string_view name, bool isMandatory, std::string_view help, action_with_arguments_t action)
        -> CommandLineParser&;

    auto addOption(std::string_view name, std::string_view help, action_t action) -> CommandLineParser&;

   private:
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};
};

}  // namespace nbn::core
