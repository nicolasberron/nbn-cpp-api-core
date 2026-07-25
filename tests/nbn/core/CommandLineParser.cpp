#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <nbn/core/utils.h>

#include <nbn/core/CommandLineParser.h>
#include <nbn/core/UnitTests.h>

using namespace nbn::core;

class TestFunctionTypeid {
   public:
    void void_function_of_void();
    void void_function_of_string(std::string);
    void void_function_of_const_string(const std::string);
    void void_function_of_const_string_ref(const std::string&);
    void void_function_of_string_view(std::string_view);
    void void_function_of_string_view_and_int(std::string_view, int);
    void void_function_of_shared_ptr(std::shared_ptr<int>);

    int int_function_of_void();
    int int_function_of_string(std::string);
    int int_function_of_string_view(std::string_view);
    int int_function_of_string_view_and_int(std::string_view, int);
    int int_function_of_shared_ptr(std::shared_ptr<int>);

    // Test for same name and different arguments
    void void_same_name_different_args(int);
    // void void_same_name_different_args(std::string_view);
    // void void_same_name_different_args(int, std::string_view);
};

void test_function_typeid() {
    std::cout << "void_function_of_void typeid: " << utils::abi::getTypeName<decltype(&TestFunctionTypeid::void_function_of_void)>()
              << std::endl;
    std::cout << "void_function_of_string typeid: "
              << utils::abi::getTypeName<decltype(&TestFunctionTypeid::void_function_of_string)>() << std::endl;
    std::cout << "void_function_of_const_string typeid: "
              << utils::abi::getTypeName<decltype(&TestFunctionTypeid::void_function_of_const_string)>() << std::endl;
    std::cout << "void_function_of_const_string_ref typeid: "
              << utils::abi::getTypeName<decltype(&TestFunctionTypeid::void_function_of_const_string_ref)>() << std::endl;
    std::cout << "void_function_of_string_view typeid: "
              << utils::abi::getTypeName<decltype(&TestFunctionTypeid::void_function_of_string_view)>() << std::endl;
    std::cout << "void_function_of_string_view_and_int typeid: "
              << utils::abi::getTypeName<decltype(&TestFunctionTypeid::void_function_of_string_view_and_int)>() << std::endl;
    std::cout << "void_function_of_shared_ptr typeid: "
              << utils::abi::getTypeName<decltype(&TestFunctionTypeid::void_function_of_shared_ptr)>() << std::endl;

    std::cout << "int_function_of_void typeid: " << utils::abi::getTypeName<decltype(&TestFunctionTypeid::int_function_of_void)>()
              << std::endl;
    std::cout << "int_function_of_string typeid: "
              << utils::abi::getTypeName<decltype(&TestFunctionTypeid::int_function_of_string)>() << std::endl;
    std::cout << "int_function_of_string_view typeid: "
              << utils::abi::getTypeName<decltype(&TestFunctionTypeid::int_function_of_string_view)>() << std::endl;
    std::cout << "int_function_of_string_view_and_int typeid: "
              << utils::abi::getTypeName<decltype(&TestFunctionTypeid::int_function_of_string_view_and_int)>() << std::endl;
    std::cout << "int_function_of_shared_ptr typeid: "
              << utils::abi::getTypeName<decltype(&TestFunctionTypeid::int_function_of_shared_ptr)>() << std::endl;

    // Test for same name and different arguments
    std::cout << "void_same_name_different_args typeid: "
              << utils::abi::getTypeName<decltype(&TestFunctionTypeid::void_same_name_different_args)>() << std::endl;
    // std::cout << "void_same_name_different_args typeid: "
    //           << utils::abi::getTypeName<decltype(&TestFunctionTypeid::void_same_name_different_args)>() << std::endl;
    // std::cout << "void_same_name_different_args typeid: "
    //           << utils::abi::getTypeName<decltype(&TestFunctionTypeid::void_same_name_different_args)>() << std::endl;
}

/*
Output with gcc13:
void_function_of_void typeid: void (TestFunctionTypeid::*)()
void_function_of_string typeid: void (TestFunctionTypeid::*)(std::__cxx11::basic_string<char, std::char_traits<char>,
std::allocator<char> >) void_function_of_const_string typeid: void (TestFunctionTypeid::*)(std::__cxx11::basic_string<char,
std::char_traits<char>, std::allocator<char> >) void_function_of_const_string_ref typeid: void
(TestFunctionTypeid::*)(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
void_function_of_string_view typeid: void (TestFunctionTypeid::*)(std::basic_string_view<char, std::char_traits<char> >)
void_function_of_string_view_and_int typeid: void (TestFunctionTypeid::*)(std::basic_string_view<char, std::char_traits<char> >,
int) void_function_of_shared_ptr typeid: void (TestFunctionTypeid::*)(std::shared_ptr<int>) int_function_of_void typeid: int
(TestFunctionTypeid::*)() int_function_of_string typeid: int (TestFunctionTypeid::*)(std::__cxx11::basic_string<char,
std::char_traits<char>, std::allocator<char> >) int_function_of_string_view typeid: int
(TestFunctionTypeid::*)(std::basic_string_view<char, std::char_traits<char> >) int_function_of_string_view_and_int typeid: int
(TestFunctionTypeid::*)(std::basic_string_view<char, std::char_traits<char> >, int) int_function_of_shared_ptr typeid: int
(TestFunctionTypeid::*)(std::shared_ptr<int>)

Output with msvc:
void_function_of_void typeid: void (__thiscall TestFunctionTypeid::*)()
or
void_function_of_void typeid: void (__thiscall TestFunctionTypeid::*)(void)
The same for all void functions
void_function_of_string typeid: void (__thiscall TestFunctionTypeid::*)(std::basic_string<char, std::char_traits<char>,
std::allocator<char> >) void_function_of_const_string typeid: void (__thiscall TestFunctionTypeid::*)(std::basic_string<char,
std::char_traits<char>, std::allocator<char> > const) void_function_of_const_string_ref typeid: void (__thiscall
TestFunctionTypeid::*)(std::basic_string<char, std::char_traits<char>, std::allocator<char> > const &) void_function_of_string_view
typeid: void (__thiscall TestFunctionTypeid::*)(std::basic_string_view<char, std::char_traits<char> >)
void_function_of_string_view_and_int typeid: void (__thiscall TestFunctionTypeid::*)(std::basic_string_view<char,
std::char_traits<char> >, int) void_function_of_shared_ptr typeid: void (__thiscall TestFunctionTypeid::*)(std::shared_ptr<int>)
int_function_of_void typeid: int (__thiscall TestFunctionTypeid::*)(void)
int_function_of_string typeid: int (__thiscall TestFunctionTypeid::*)(std::basic_string<char, std::char_traits<char>,
std::allocator<char> >) int_function_of_string_view typeid: int (__thiscall TestFunctionTypeid::*)(std::basic_string_view<char,
std::char_traits<char> >) int_function_of_string_view_and_int typeid: int (__thiscall
TestFunctionTypeid::*)(std::basic_string_view<char, std::char_traits<char> >, int) int_function_of_shared_ptr typeid: int
(__thiscall TestFunctionTypeid::*)(std::shared_ptr<int>)

*/

void test_command_line_parser() {
    // Test data
    std::vector<std::string> args = {"program", "--option1", "value1", "--option2"};
    CommandLineParser parser(args, "Test help message");

    // Test states
    bool isOption1Called = false;
    bool isOption2Called = false;
    std::string option1Value;

    // Setup parser with options and arguments
    parser.addOption("option1", true, "Option 1 help message", [&](std::string_view value) {
        isOption1Called = true;
        option1Value = value;
    });
    parser.addOption("option2", "Option 2 help message", [&]() { isOption2Called = true; });

    // Execute parsing
    int parseResult = parser.parse();

    // Check results
    unit_tests::equal("CommandLineParser::parse() should return 0 on success", 0, parseResult);
    unit_tests::isTrue("Option1 action should be called", isOption1Called);
    unit_tests::equal("Option1 should receive 'value1' as its argument", "value1", option1Value);
    unit_tests::isTrue("Option2 action should be called", isOption2Called);

    // Test mandatory option
    std::vector<std::string> argsMissingMandatory = {"program", "--option2"};
    CommandLineParser parserMissingMandatory(argsMissingMandatory, "Test help message");
    parserMissingMandatory.addOption("option1", true, "Option 1 help message", [](std::string_view) {});
    parserMissingMandatory.addOption("option2", "Option 2 help message", []() {});

    // Execute parsing with missing mandatory option
    int parseMissingMandatoryResult = parserMissingMandatory.parse();

    // Check result for missing mandatory option
    unit_tests::notEqual("CommandLineParser::parse() should return non-zero when a mandatory option is missing", 0,
                         parseMissingMandatoryResult);

    // Test unknown option
    std::vector<std::string> argsUnknownOption = {"program", "--unknown"};
    CommandLineParser parserUnknownOption(argsUnknownOption, "Test help message");
    parserUnknownOption.addOption("option1", true, "Option 1 help message", [](std::string_view) {});
    parserUnknownOption.addOption("option2", "Option 2 help message", []() {});

    // Execute parsing with unknown option
    int parseUnknownOptionResult = parserUnknownOption.parse();

    // Check result for unknown option
    unit_tests::notEqual("CommandLineParser::parse() should return non-zero when an unknown option is provided", 0,
                         parseUnknownOptionResult);
}

void test_command_line_parser_handles_values_arguments_and_formatting() {
    std::vector<std::string> args{"program", "--value", "42", "first", "second"};
    constexpr auto kFourtyColumns = 40U;
    CommandLineParser parser{args, "coverage help", kFourtyColumns};
    std::string value;
    std::vector<std::string> positional;
    parser.addOption("value", false, "A value option", [&value](std::string_view input) { value = input; });
    parser.addAnyArgumentsSuffix([&positional](std::string_view input) { positional.emplace_back(input); });

    unit_tests::equal("Value option should parse", 0, parser.parse());
    unit_tests::equal("Value option should receive its argument", std::string{"42"}, value);
    unit_tests::equal("Any-argument suffix should receive all positional values", static_cast<std::size_t>(2), positional.size());
    parser.printHelp();

    const auto missingValue = std::vector<std::string>{"program", "--value"};
    CommandLineParser missingValueParser{missingValue, "coverage help"};
    missingValueParser.addOption("value", false, "A value option", [](std::string_view) {});
    unit_tests::notEqual("Missing option value should fail parsing", 0, missingValueParser.parse());

    const auto invalidValue = std::vector<std::string>{"program", "--value", "--help"};
    CommandLineParser invalidValueParser{invalidValue, "coverage help"};
    invalidValueParser.addOption("value", false, "A value option", [](std::string_view) {});
    unit_tests::notEqual("Option used as a value should fail parsing", 0, invalidValueParser.parse());
}

void test_command_line_parser_handles_named_arguments_and_duplicate_options() {
    constexpr auto kHelpColumns = 40U;
    const std::vector<std::string> arguments{"program", "input.json"};
    CommandLineParser parser{arguments, "coverage help", kHelpColumns};
    std::string parsedArgument;
    parser.addArgument("input", true, "A deliberately long argument description that wraps across lines",
                       [&parsedArgument](std::string_view value) { parsedArgument = value; });

    unit_tests::equal("Named argument should parse", 0, parser.parse());
    unit_tests::equal("Named argument should receive its value", std::string{"input.json"}, parsedArgument);

    const std::vector<std::string> missingArgument{"program"};
    CommandLineParser missingArgumentParser{missingArgument, "coverage help"};
    missingArgumentParser.addArgument("input", true, "Input file", [](std::string_view) {});
    unit_tests::notEqual("Missing mandatory argument should fail parsing", 0, missingArgumentParser.parse());

    const std::vector<std::string> duplicateOption{"program", "--flag", "--flag"};
    CommandLineParser duplicateOptionParser{duplicateOption, "coverage help"};
    duplicateOptionParser.addOption("flag", "A flag", []() {});
    unit_tests::notEqual("Duplicate option should fail parsing", 0, duplicateOptionParser.parse());
}
