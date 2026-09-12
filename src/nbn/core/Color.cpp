#include "Color.h"

#include <exception>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace nbn::core {

namespace {

constexpr std::size_t kHexRgbLength{7U};
constexpr std::size_t kHexRgbaLength{9U};
constexpr std::size_t kHexComponentWidth{2U};
constexpr std::size_t kRedOffset{1U};
constexpr std::size_t kGreenOffset{3U};
constexpr std::size_t kBlueOffset{5U};
constexpr std::size_t kAlphaOffset{7U};
constexpr int kHexBase{16};
constexpr unsigned char kDefaultAlpha{255U};
constexpr char kHexPrefix{'#'};

struct Rgba {
    unsigned char red{};
    unsigned char green{};
    unsigned char blue{};
    unsigned char alpha{};
};

struct RgbaProperties {
    property_ptr<unsigned char> red;
    property_ptr<unsigned char> green;
    property_ptr<unsigned char> blue;
    property_ptr<unsigned char> alpha;
};

struct SynchronizationReset {
    explicit SynchronizationReset(bool* pIsSynchronizing) : m_pIsSynchronizing(pIsSynchronizing) {}

    ~SynchronizationReset() {
        if (m_pIsSynchronizing != nullptr) {
            *m_pIsSynchronizing = false;
        }
    }

   private:
    bool* m_pIsSynchronizing;
};

auto getHex(const Rgba& rgba) -> std::string {
    return std::format("#{0:02X}{1:02X}{2:02X}{3:02X}", rgba.red, rgba.green, rgba.blue, rgba.alpha);
}

auto parseHexComponent(std::string_view hex, std::size_t offset) -> unsigned char {
    return static_cast<unsigned char>(std::stoul(std::string{hex.substr(offset, kHexComponentWidth)}, nullptr, kHexBase));
}

auto parseHex(std::string_view hex) -> std::optional<Rgba> {
    if (hex.size() != kHexRgbLength && hex.size() != kHexRgbaLength) {
        return std::nullopt;
    }
    if (hex[0] != kHexPrefix) {
        return std::nullopt;
    }

    try {
        Rgba rgba{};
        rgba.red = parseHexComponent(hex, kRedOffset);
        rgba.green = parseHexComponent(hex, kGreenOffset);
        rgba.blue = parseHexComponent(hex, kBlueOffset);
        rgba.alpha = (hex.size() == kHexRgbaLength) ? parseHexComponent(hex, kAlphaOffset) : kDefaultAlpha;
        return rgba;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

auto onRgbOrAlphaChanged(const Rgba& rgba, property_ptr<std::string> hexProperty, bool& isSynchronizing) -> void {
    if (isSynchronizing) {
        return;
    }

    isSynchronizing = true;
    const SynchronizationReset resetSynchronization{&isSynchronizing};
    hexProperty->set(getHex(rgba));
}

auto onHexChanged(std::string_view hex, const RgbaProperties& properties, bool& isSynchronizing) -> void {
    if (isSynchronizing) {
        return;
    }

    const auto rgba = parseHex(hex);
    if (rgba.has_value()) {
        isSynchronizing = true;
        const SynchronizationReset resetSynchronization{&isSynchronizing};
        properties.red->set(rgba->red);
        properties.green->set(rgba->green);
        properties.blue->set(rgba->blue);
        properties.alpha->set(rgba->alpha);
    }
}

}  // namespace

// Factory registration
static auto const registerToFactory{[]() { return nbn::core::Object::factoryRegister<Color>(); }()};

class Color::Impl {
   public:
    bool isSynchronizing{false};
};

Color::Color() : m_spImpl(std::make_unique<Impl>()) {
    red()->signalValueChanged()->connect([this]([[maybe_unused]] unsigned char oldRed, unsigned char red) {
        onRgbOrAlphaChanged(Rgba{red, green()->get(), blue()->get(), alpha()->get()}, hex(), m_spImpl->isSynchronizing);
    });

    green()->signalValueChanged()->connect([this]([[maybe_unused]] unsigned char oldGreen, unsigned char green) {
        onRgbOrAlphaChanged(Rgba{red()->get(), green, blue()->get(), alpha()->get()}, hex(), m_spImpl->isSynchronizing);
    });

    blue()->signalValueChanged()->connect([this]([[maybe_unused]] unsigned char oldBlue, unsigned char blue) {
        onRgbOrAlphaChanged(Rgba{red()->get(), green()->get(), blue, alpha()->get()}, hex(), m_spImpl->isSynchronizing);
    });

    alpha()->signalValueChanged()->connect([this]([[maybe_unused]] unsigned char oldAlpha, unsigned char alpha) {
        onRgbOrAlphaChanged(Rgba{red()->get(), green()->get(), blue()->get(), alpha}, hex(), m_spImpl->isSynchronizing);
    });

    hex()->signalValueChanged()->connect([this]([[maybe_unused]] std::string oldHex, std::string hex) {
        onHexChanged(hex, RgbaProperties{red(), green(), blue(), alpha()}, m_spImpl->isSynchronizing);
    });
}

Color::~Color() = default;

auto Color::red() -> property_ptr<unsigned char> {
    return property<unsigned char>();
}

auto Color::green() -> property_ptr<unsigned char> {
    return property<unsigned char>();
}

auto Color::blue() -> property_ptr<unsigned char> {
    return property<unsigned char>();
}

auto Color::alpha() -> property_ptr<unsigned char> {
    return property<unsigned char>();
}

auto Color::hex() -> property_ptr<std::string> {
    return property<std::string>();
}

}  // namespace nbn::core