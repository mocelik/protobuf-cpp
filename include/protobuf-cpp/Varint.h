#pragma once

#include <cstddef> // std::byte
#include <cstdint>
#include <span>
#include <vector>

namespace proto {

class Varint {
  public:
    constexpr Varint(std::uint64_t value) noexcept : m_value(value) {}

    constexpr static Varint
    deserialize(std::span<const std::byte> data) noexcept {
        constexpr std::byte continue_bit{0b1000'0000};
        std::uint64_t result = 0;
        int shift = 0;
        for (auto byte : data) {
            result |= (static_cast<std::uint64_t>(byte & std::byte{0b0111'1111})
                       << shift);
            if (!bool(byte & continue_bit)) {
                break;
            }
            shift += 7;
        }
        return Varint{result};
    }

    [[nodiscard]] constexpr std::uint64_t value() const noexcept {
        return m_value;
    }
    constexpr void set_value(std::uint64_t value) noexcept { m_value = value; }

    constexpr std::vector<std::byte> serialize() const {
        constexpr std::byte continue_bit{0b1000'0000};
        auto value = m_value;
        std::vector<std::byte> result;
        do {
            std::byte byte = static_cast<std::byte>(value & 0x7f);
            value >>= 7;
            if (value != 0) {
                byte |= continue_bit;
            }
            result.push_back(byte);
        } while (value != 0);
        return result;
    }

    operator std::uint64_t() const noexcept { return m_value; }

  private:
    std::uint64_t m_value;
};

} // namespace proto
