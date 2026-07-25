#include <string>
#include <vector>

#include <nbn/core/CommandLineParser.h>

int main() {
    const std::vector<std::string> arguments{"command-line-parser-help", "--help"};
    nbn::core::CommandLineParser parser{arguments, "CommandLineParser help coverage"};
    return parser.parse();
}
