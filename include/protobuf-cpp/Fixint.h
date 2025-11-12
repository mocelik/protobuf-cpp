#pragma once

#include "Deserialized.h"
#include "WireType.h"

#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <limits>
#include <span>
#include <stdexcept>
#include <type_traits>

namespace proto {

template <typename NumericType> class Fixint {
  public:
    static_assert(std::is_arithmetic_v<NumericType>,
                  "Fixint only supports arithmetic types");

    static_assert(!std::is_floating_point_v<NumericType> ||
                      std::numeric_limits<NumericType>::is_iec559,
                  "Fixint only supports IEEE 754 floating point types");

    static_assert(std::endian::native == std::endian::little,
                  "Fixint only supports little-endian architectures");

    using FixedType =
        std::conditional_t<(sizeof(NumericType) <= sizeof(std::uint32_t)),
                           std::uint32_t, std::uint64_t>;
    static constexpr std::size_t kfixed_size = sizeof(FixedType);

    static constexpr WireType k_wire_type =
        kfixed_size == 4 ? WireType::FIXED32 : WireType::FIXED64;

    constexpr Fixint() = default;
    constexpr explicit Fixint(NumericType value) noexcept : m_value(value) {}

    [[nodiscard]] static Deserialized<Fixint>
    deserialize(std::span<const std::byte> data) noexcept {

        if (data.size() < kfixed_size) { // error: not enough data
            return Deserialized(Fixint{}, 0);
        }
        NumericType value;
        std::memcpy(&value, data.data(), kfixed_size);
        return Deserialized(Fixint{value}, kfixed_size);
    }

    [[nodiscard]] constexpr NumericType value() const noexcept {
        return m_value;
    }

    constexpr void set_value(NumericType value) noexcept { m_value = value; }

    [[nodiscard]] std::array<std::byte, kfixed_size> serialize() const {
        std::array<std::byte, kfixed_size> result;
        serialize(result);
        return result;
    }

    constexpr std::size_t serialize(std::span<std::byte> buffer) const {
        if (buffer.size() < size()) {
            throw std::runtime_error("Buffer too small to serialize Fixint");
        }

        FixedType value = std::bit_cast<FixedType>(m_value);
        for (int i = 0; i < kfixed_size; i++) {
            buffer[i] = static_cast<std::byte>(value);
            value >>= 8;
        }
        return kfixed_size;
    }

    [[nodiscard]] constexpr std::size_t size() const noexcept {
        return kfixed_size;
    }

  private:
    NumericType m_value{};
};
} // namespace proto
