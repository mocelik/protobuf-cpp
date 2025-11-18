#pragma once

#include "Deserialized.h"
#include "WireType.h"

#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace proto {

class Varint {
  public:
    static constexpr WireType k_wire_type = WireType::VARINT;

    constexpr Varint() = default;

    template <typename T>
        requires std::is_integral_v<T>
    constexpr explicit Varint(T value) noexcept
        : m_value(std::is_signed_v<T>
                      ? (value << 1) ^ (value >> ((sizeof(T) * 8) - 1))
                      : value) {}

    template <typename T>
        requires std::is_integral_v<T>
    T as() const {
        if constexpr (std::is_signed_v<T>) {
            return (m_value >> 1) ^ (-(m_value & 1));
        }

        // Cast to suppress warnings about precision loss
        return static_cast<T>(m_value);
    }

    [[nodiscard]] constexpr static Deserialized<Varint>
    deserialize(std::span<const std::byte> data) noexcept {
        constexpr std::byte continue_mask{0b1000'0000};
        constexpr std::byte value_mask{0b0111'1111};
        std::uint64_t result = 0;
        std::size_t num_bytes_read = 0;
        int shift = 0;
        for (auto byte : data) {
            result |=
                (std::to_integer<std::uint64_t>(byte & value_mask) << shift);
            num_bytes_read++;
            if (!bool(byte & continue_mask)) {
                break;
            }
            shift += 7;
        }
        return Deserialized{Varint{result}, num_bytes_read};
    }

    [[nodiscard]] constexpr std::uint64_t value() const noexcept {
        return m_value;
    }
    constexpr void set_value(std::uint64_t value) noexcept { m_value = value; }

    constexpr std::size_t serialize(std::span<std::byte> buffer) const {
        if (buffer.size() < size()) {
            throw std::runtime_error("Not enough space in provided span");
        }

        constexpr std::byte continue_mask{0b1000'0000};
        constexpr std::byte value_mask{0b0111'1111};
        auto value = m_value;
        std::size_t num_bytes_written{0};
        do {
            std::byte byte = static_cast<std::byte>(value) & value_mask;
            value >>= 7;
            if (value != 0) {
                byte |= continue_mask;
            }
            buffer[num_bytes_written] = byte;
            num_bytes_written++;
        } while (value != 0);
        return num_bytes_written;
    }

    [[nodiscard]] constexpr std::size_t size() const noexcept {
        // special case
        if (m_value == 0) {
            return 1;
        }
        // 7 bits per byte
        return (std::bit_width(m_value) + 6) / 7;
    }

  private:
    std::uint64_t m_value{};
};

} // namespace proto
