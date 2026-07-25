#include <algorithm>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_set>

#include "CommandLineParser.h"

namespace nbn::core {

class CommandLineParser::Impl {
   public:
    Impl(CommandLineParser* pDecl, const std::vector<std::string>& args, std::string_view helpMessage, size_t helpMaxColumns)
        : m_pDecl(pDecl),
          m_argv(args),
          m_helpMessage(helpMessage),
          m_helpMaxColumns(helpMaxColumns),
          m_mandatoryArgumentsOrOptions{} {}

    auto printHelp(std::ostream& out = std::cout) const -> void {
        auto widestOptionOrArgument{
            std::accumulate(m_actions.begin(), m_actions.end(),
                            std::accumulate(m_actionsWithArgument.begin(), m_actionsWithArgument.end(), size_t{0},
                                            [](size_t max, const auto& action) { return std::max(max, action.first.size()); }),
                            [](size_t max, const auto& action) { return std::max(max, action.first.size()); })};

        out << m_helpMessage << std::endl;
        out << "Usage:" << std::endl;

        std::vector<std::string> allActionsNames;
        allActionsNames.reserve(m_actions.size() + m_actionsWithArgument.size());
        std::transform(m_actions.begin(), m_actions.end(), std::back_inserter(allActionsNames),
                       [](const auto& pair) { return pair.first; });
        std::transform(m_actionsWithArgument.begin(), m_actionsWithArgument.end(), std::back_inserter(allActionsNames),
                       [](const auto& pair) { return pair.first; });

        // Remove arguments from allActionsNames
        allActionsNames.erase(
            std::remove_if(allActionsNames.begin(), allActionsNames.end(), [this](const auto& name) { return isArgument(name); }),
            allActionsNames.end());

        // Sort allActionsNames
        std::sort(allActionsNames.begin(), allActionsNames.end());

        // Add arguments at the end of allActionsNames
        std::copy_if(m_argumentsNames.begin(), m_argumentsNames.end(), std::back_inserter(allActionsNames),
                     [this](const auto& name) { return isArgument(name); });

        auto usageString{std::accumulate(allActionsNames.begin(), allActionsNames.end(), std::string{},
                                         [this](const std::string& usage, const std::string& name) {
                                             return std::format("{}{}", usage, toUsageString(name, [this](auto name) {
                                                                    return isOption(name) ? optionToUsageString(name)
                                                                                          : argumentToUsageString(name);
                                                                }));
                                         })};

        auto programName{std::filesystem::path(m_argv[0]).filename().string()};
        printHelpFor(programName, usageString, programName.size(), out);

        // Print options and arguments
        out << std::endl << "Options:" << std::endl;
        for (const auto& name : allActionsNames) {
            if (isOption(name)) {
                printHelpFor(name, m_helps.at(name), widestOptionOrArgument, out);
            }
        }

        if (!m_argumentsNames.empty()) {
            out << std::endl << "Arguments:" << std::endl;
            for (const auto& name : m_argumentsNames) {
                printHelpFor(name, m_helps.at(name), widestOptionOrArgument, out);
            }
        }
    }

    [[nodiscard]] auto parse() -> int {
        std::vector<std::string> arguments;
        std::unordered_set<std::string> parsedOptions;

        for (size_t i{1}; i < m_argv.size(); ++i) {
            const std::string& arg = m_argv[i];

            if (arg[0] == '-' && arg.length() > 1 && arg[1] == '-') {  // option
                if (!isRegistered(arg)) {
                    return printError(std::format("Unknown option: {}", arg));
                }

                if (!hasValue(arg)) {
                    m_actions[arg]();
                } else {
                    if (i + 1 >= m_argv.size()) {
                        return printError(std::format("Missing value for option: {}", arg));
                    }

                    // If the next argument is an option, then it is an invalid value for the current option
                    auto nextArg{m_argv[i + 1]};
                    if (nextArg.compare(0, 2, "--") == 0) {
                        return printError(std::format("Invalid value for option: {} {}", arg, nextArg));
                    }

                    m_actionsWithArgument[arg](m_argv[++i]);
                }

                if (!parsedOptions.emplace(arg).second) {
                    return printError(std::format("Option {} is already set", arg));
                }
            } else {  // argument
                arguments.emplace_back(arg);
            }
        }

        // Check if all mandatory options/arguments are set
        for (const auto& argumentOrOption : m_mandatoryArgumentsOrOptions) {
            if (argumentOrOption[0] == '-' && parsedOptions.find(argumentOrOption) == parsedOptions.end()) {
                return printError(std::format("Missing mandatory option: {}", argumentOrOption));
            }
        }

        // Process arguments
        if (!m_argumentsNames.empty()) {
            for (size_t i{0}; i < m_argumentsNames.size(); ++i) {
                const auto& argumentName{m_argumentsNames[i]};
                if (i < arguments.size()) {
                    if (argumentName == m_anyArgumentsSuffix) {
                        for (size_t j{i}; j < arguments.size(); ++j) {
                            m_actionsWithArgument[m_anyArgumentsSuffix](arguments[j]);
                        }
                        break;
                    }
                    m_actionsWithArgument[argumentName](arguments[i]);
                } else if (isMandatory(argumentName)) {
                    return printError(std::format("Missing mandatory argument: {}", argumentName));
                }
            }
        }

        return 0;
    }

    auto addArgument(std::string_view name, bool isMandatory, std::string_view help, action_with_arguments_t action)
        -> CommandLineParser& {
        std::string nameStr{name};  // Convert to string for storage
        m_argumentsNames.emplace_back(nameStr);
        m_actionsWithArgument[nameStr] = std::move(action);
        if (isMandatory) {
            m_mandatoryArgumentsOrOptions.insert(nameStr);
        }
        m_helps.emplace(nameStr, help);
        return *m_pDecl;
    }

    auto addAnyArgumentsSuffix(action_with_arguments_t action) -> CommandLineParser& {
        return addArgument(m_anyArgumentsSuffix, false, "Any number of same arguments same as the previous one.",
                           std::move(action));
    }

    auto addOption(std::string_view name, bool isMandatory, std::string_view help, action_with_arguments_t action)
        -> CommandLineParser& {
        std::string optionName{"--" + std::string{name}};
        m_actionsWithArgument.emplace(optionName, std::move(action));
        if (isMandatory) {
            m_mandatoryArgumentsOrOptions.emplace(optionName);
        }
        m_helps.emplace(optionName, help);
        return *m_pDecl;
    }

    auto addOption(std::string_view name,  // NOLINT(bugprone-easily-swappable-parameters)
                   std::string_view help,
                   action_t action) -> CommandLineParser& {
        std::string optionName{"--" + std::string{name}};
        m_actions.emplace(optionName, std::move(action));
        m_helps.emplace(optionName, help);
        return *m_pDecl;
    }

   private:
    auto isOption(std::string_view name) const -> bool {
        std::string nameStr{name};
        return !isArgument(name) &&
               (m_actions.find(nameStr) != m_actions.end() || m_actionsWithArgument.find(nameStr) != m_actionsWithArgument.end());
    }

    auto isArgument(std::string_view name) const -> bool {
        return std::find(m_argumentsNames.begin(), m_argumentsNames.end(), name) != m_argumentsNames.end();
    }

    auto isMandatory(std::string_view name) const -> bool {
        return m_mandatoryArgumentsOrOptions.find(std::string(name)) != m_mandatoryArgumentsOrOptions.end();
    }

    auto isRegistered(std::string_view name) const -> bool {
        std::string nameStr{name};
        return m_actionsWithArgument.count(nameStr) > 0 || m_actions.count(nameStr) > 0;
    }

    auto hasValue(std::string_view name) const -> bool {
        std::string nameStr{name};
        return m_actionsWithArgument.count(nameStr) > 0;
    }

    auto toUsageString(std::string_view name, std::function<std::string(const std::string&)> getUsageString) const -> std::string {
        auto usageString{getUsageString(std::string(name))};
        return isMandatory(name) ? std::format(" {}", usageString) : std::format(" [{}]", usageString);
    }

    auto optionToUsageString(const std::string& name) const -> std::string {
        if (hasValue(name)) {
            return std::format("{} <value>", name);
        }
        return std::string{name};
    }

    auto argumentToUsageString(const std::string& name) const -> std::string { return std::string{name}; }

    auto printHelpFor(std::string_view name,  // NOLINT(bugprone-easily-swappable-parameters)
                      std::string_view help,
                      size_t widestName,
                      std::ostream& out) const -> void {
        auto nameColumnWidth{widestName + 2};
        auto helpColumnStart{nameColumnWidth + 4};
        auto helpColumnWidth{m_helpMaxColumns - helpColumnStart};

        out << "  " << name << std::string(nameColumnWidth - name.size(), ' ');

        size_t currentWidth{0};
        std::string helpStr{help};
        std::istringstream words(helpStr);
        std::string word;
        while (words >> word) {
            if (currentWidth + word.length() > helpColumnWidth) {
                out << std::endl << std::string(helpColumnStart, ' ');
                currentWidth = 0;
            }
            out << word << ' ';
            currentWidth += word.length() + 1;
        }
        out << std::endl;
    }

    auto printError(std::string_view message) const -> int {
        std::cerr << message << std::endl;
        printHelp(std::cerr);
        return 1;
    }

    CommandLineParser* m_pDecl{nullptr};
    std::vector<std::string> m_argv;
    std::string m_helpMessage;
    size_t m_helpMaxColumns;
    std::vector<std::string> m_argumentsNames;
    std::unordered_map<std::string, action_with_arguments_t> m_actionsWithArgument;
    std::unordered_map<std::string, action_t> m_actions;
    std::unordered_map<std::string, std::string> m_helps;
    std::unordered_set<std::string> m_mandatoryArgumentsOrOptions;
    std::string m_anyArgumentsSuffix{"..."};
};

// --------------------------------------------------------------------------------------------
// CommandLineParser
// --------------------------------------------------------------------------------------------

CommandLineParser::CommandLineParser(const std::vector<std::string>& args, std::string_view help, size_t helpMaxColumns)
    : m_spImpl(std::make_unique<Impl>(this, args, help, helpMaxColumns)) {
    addOption("help", "Print this help message", [&]() {
        m_spImpl->printHelp(std::cout);
        exit(0);
    });
}

CommandLineParser::~CommandLineParser() = default;

void CommandLineParser::printHelp() const {
    m_spImpl->printHelp();
}

auto CommandLineParser::parse() -> int {
    return m_spImpl->parse();
}

auto CommandLineParser::addArgument(std::string_view name, bool isMandatory, std::string_view help, action_with_arguments_t action)
    -> CommandLineParser& {
    return m_spImpl->addArgument(name, isMandatory, help, std::move(action));
}

auto CommandLineParser::addAnyArgumentsSuffix(action_with_arguments_t action) -> CommandLineParser& {
    return m_spImpl->addAnyArgumentsSuffix(std::move(action));
}

auto CommandLineParser::addOption(std::string_view name, bool isMandatory, std::string_view help, action_with_arguments_t action)
    -> CommandLineParser& {
    return m_spImpl->addOption(name, isMandatory, help, std::move(action));
}

auto CommandLineParser::addOption(std::string_view name, std::string_view help, action_t action) -> CommandLineParser& {
    return m_spImpl->addOption(name, help, std::move(action));
}
}  // namespace nbn::core