#pragma once

#include "Deserialized.h"
#include "WireType.h"

#include <concepts>
#include <cstddef>
#include <span>

namespace proto {
template <class T>
concept Wirable = requires(T t, std::span<std::byte> buffer) {
    { T::k_wire_type } -> std::convertible_to<WireType>;
    { t.size() } -> std::convertible_to<std::size_t>;
    { t.serialize(buffer) } -> std::convertible_to<std::size_t>;
    { t.deserialize(buffer) } -> std::convertible_to<Deserialized<T>>;
};

template <typename WiredType, typename T>
concept InterpretableAs = Wirable<WiredType> && requires(WiredType wire) {
    { wire.template as<T>() } -> std::same_as<T>;
};

} // namespace proto
