#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>

#include <nbn/core/Application.h>
#include <nbn/core/detail/serialization.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: json_parser_probe <input-file>\n";
        return 2;
    }

    nbn::core::Application::init({argv[0]});

    std::ifstream input(argv[1], std::ios::binary);
    const std::string json{std::istreambuf_iterator<char>{input}, std::istreambuf_iterator<char>{}};
    try {
        if (json.find_first_not_of(" \t\n\r") != std::string::npos && json[json.find_first_not_of(" \t\n\r")] == '{') {
            std::unordered_map<std::string, std::string> properties;
            size_t position{0};
            nbn::core::serialization::json::parseObject(properties, json, position);
            nbn::core::serialization::json::validateEnd(json, position);
        } else {
            (void)nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(json);
        }
    } catch (...) {
        nbn::core::Application::quit(1);
        return 1;
    }
    nbn::core::Application::quit(0);
    return 0;
}
