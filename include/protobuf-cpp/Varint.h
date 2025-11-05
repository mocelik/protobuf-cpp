#pragma once

#include "Deserialized.h"
#include "WireType.h"

#include <cstddef> // std::byte
#include <cstdint>
#include <span>
#include <vector>

namespace proto {

class Varint {
  public:
    static constexpr WireType k_wire_type = WireType::VARINT;

    constexpr Varint() = default;
    constexpr explicit Varint(std::uint64_t value) noexcept : m_value(value) {}

    [[nodiscard]] constexpr static Deserialized<Varint>
    deserialize(std::span<const std::byte> data) noexcept {
        constexpr std::byte continue_mask{0b1000'0000};
        constexpr std::byte value_mask{0b0111'1111};
        std::uint64_t result = 0;
        std::size_t num_bytes_read = 0;
        int shift = 0;
        for (auto byte : data) {
            result |= (static_cast<std::uint64_t>(byte & value_mask) << shift);
            num_bytes_read++;
            if (!bool(byte & continue_mask)) {
                break;
            }
            shift += 7;
        }
        return Deserialized(Varint{result}, num_bytes_read);
    }

    [[nodiscard]] constexpr std::uint64_t value() const noexcept {
        return m_value;
    }
    constexpr void set_value(std::uint64_t value) noexcept { m_value = value; }

    [[nodiscard]] constexpr std::vector<std::byte> serialize() const {
        constexpr std::byte continue_mask{0b1000'0000};
        constexpr std::byte value_mask{0b0111'1111};
        auto value = m_value;
        std::vector<std::byte> result;
        do {
            std::byte byte = static_cast<std::byte>(value) & value_mask;
            value >>= 7;
            if (value != 0) {
                byte |= continue_mask;
            }
            result.push_back(byte);
        } while (value != 0);
        return result;
    }

  private:
    std::uint64_t m_value{};
};

} // namespace proto
