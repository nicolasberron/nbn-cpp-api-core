#include <nbn/core/Color.h>

int main() {
    nbn::core::Color color{};
    return color.red() != nullptr ? 0 : 1;
}