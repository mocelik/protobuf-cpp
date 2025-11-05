#pragma once

#include "Field.h"
#include "Varint.h"
#include "WireType.h"

#include <utility>

namespace proto {
struct Key {
    constexpr explicit Key(Varint value_varint) noexcept
        : m_value(value_varint.value()) {}

    constexpr explicit Key(Field field, WireType wire_type) noexcept
        : m_value(std::to_underlying(field) << 3 |
                  std::to_underlying(wire_type)) {}

    [[nodiscard]] constexpr std::uint64_t value() const noexcept {
        return m_value;
    }
    [[nodiscard]] constexpr Field field_number() const noexcept {
        return static_cast<Field>(m_value >> 3);
    }
    [[nodiscard]] constexpr WireType wire_type() const noexcept {
        return static_cast<WireType>(m_value & 0x07);
    }

    auto operator<=>(const Key &) const = default;

  private:
    std::uint64_t m_value{};
};
} // namespace proto
