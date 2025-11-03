#pragma once

#include "Deserialized.h"

#include <array>
#include <cstdint>
#include <span>

namespace proto {

class Fixed32 {
  public:
    constexpr Fixed32(std::uint32_t value) noexcept : m_value(value) {}

    [[nodiscard]] constexpr static Deserialized<Fixed32>
    deserialize(std::span<const std::byte> data) noexcept {

        if (data.size() < 4) { // error: not enough data
            return Deserialized(Fixed32{0}, 0);
        }

        std::uint32_t result = 0;
        for (std::size_t i = 0; i < sizeof(std::uint32_t); ++i) {
            result |= (static_cast<std::uint32_t>(data[i]) << (8 * i));
        }
        return Deserialized(Fixed32{result}, sizeof(std::uint32_t));
    }

    [[nodiscard]] constexpr std::uint32_t value() const noexcept {
        return m_value;
    }

    constexpr void set_value(std::uint32_t value) noexcept { m_value = value; }

    [[nodiscard]] constexpr std::array<std::byte, sizeof(std::uint32_t)>
    serialize() const noexcept {
        std::array<std::byte, sizeof(std::uint32_t)> result;
        for (std::size_t i = 0; i < sizeof(std::uint32_t); ++i) {
            result[i] = static_cast<std::byte>((m_value >> (8 * i)) & 0xFF);
        }
        return result;
    }

  private:
    std::uint32_t m_value;
};
} // namespace proto
