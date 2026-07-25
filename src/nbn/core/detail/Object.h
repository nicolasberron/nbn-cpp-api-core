#pragma once

namespace nbn::core {

template <typename... Args>
[[nodiscard]] auto Object::signal(const std::source_location& location) -> nbn::core::signal_ptr<Args...> {
    auto spSignal = signal(location);
    if (spSignal != nullptr) {
        return std::static_pointer_cast<detail::Signal<Args...>>(spSignal);
    }

    auto spNewSignal = std::make_shared<detail::Signal<Args...>>(location.function_name(), this);
    addSignal(location, spNewSignal);
    return spNewSignal;
}

template <typename U>
[[nodiscard]] auto Object::property(const std::source_location& location) -> nbn::core::property_ptr<U> {
    auto spProperty = property(location);
    if (spProperty != nullptr) {
        return std::static_pointer_cast<detail::Property<U>>(spProperty);
    }

    auto spNewProperty = std::make_shared<detail::Property<U>>(location.function_name(), this);
    addProperty(location, std::static_pointer_cast<nbn::core::interfaces::IProperty>(spNewProperty));
    return spNewProperty;
}

template <typename U>
[[nodiscard]] auto Object::enumProperty(const std::source_location& location) -> nbn::core::enum_property_ptr<U> {
    auto spProperty = property(location);
    if (spProperty != nullptr) {
        return std::static_pointer_cast<detail::EnumProperty<U>>(spProperty);
    }

    auto spNewProperty = std::make_shared<detail::EnumProperty<U>>(location.function_name(), this);
    addProperty(location, std::static_pointer_cast<nbn::core::interfaces::IProperty>(spNewProperty));
    return spNewProperty;
}

template <typename... Args>
[[nodiscard]] auto Object::slot(const std::source_location& location) -> nbn::core::slot_ptr<Args...> {
    auto spSlot = slot(location);
    if (spSlot != nullptr) {
        return std::static_pointer_cast<detail::Slot<Args...>>(spSlot);
    }

    auto spNewSlot = std::make_shared<detail::Slot<Args...>>(location.function_name(), this);
    addSlot(location, spNewSlot);
    return spNewSlot;
}

}  // namespace nbn::core