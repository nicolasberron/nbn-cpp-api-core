#pragma once

#include <memory>

#include "Object.h"

namespace nbn::core {

/**
 * @file Color.h
 * @brief Serializable RGBA color object.
 */

/**
 * @class Color
 * @brief Stores red, green, blue, and alpha color channels as properties.
 *
 * @par Usage example
 * @code{.cpp}
 * nbn::core::Color color;
 * color.red()->set(255);
 * color.green()->set(128);
 * color.blue()->set(0);
 * color.alpha()->set(255);
 * const auto serializedColor = color.hex()->get();
 * @endcode
 */
class Color : public Object {
   public:
    /** @brief Constructs a color with its default channel values. */
    Color();
    /** @brief Destroys the color object. */
    ~Color() override;

    /** @brief Returns the red channel property. */
    [[nodiscard]] auto red() -> property_ptr<unsigned char>;
    /** @brief Returns the green channel property. */
    [[nodiscard]] auto green() -> property_ptr<unsigned char>;
    /** @brief Returns the blue channel property. */
    [[nodiscard]] auto blue() -> property_ptr<unsigned char>;
    /** @brief Returns the alpha channel property. */
    [[nodiscard]] auto alpha() -> property_ptr<unsigned char>;

    /** @brief Returns the hexadecimal color representation property. */
    [[nodiscard]] auto hex() -> property_ptr<std::string>;

   private:
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};
};

}  // namespace nbn::core