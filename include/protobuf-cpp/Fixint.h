#pragma once

#include "Deserialized.h"
#include "WireType.h"

#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <limits>
#include <ranges>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace proto {

template <typename UnderlyingType>
    requires std::is_same_v<UnderlyingType, std::uint32_t> ||
             std::is_same_v<UnderlyingType, std::uint64_t>
class Fixint {
  public:
    static_assert(std::endian::native == std::endian::little,
                  "Fixint only supports little-endian architectures");

    static constexpr WireType k_wire_type =
        std::is_same_v<UnderlyingType, std::uint32_t> ? WireType::FIXED32
                                                      : WireType::FIXED64;

    constexpr Fixint() = default;

    template <typename IntegralType>
        requires std::is_integral_v<IntegralType> &&
                 (sizeof(IntegralType) <= sizeof(UnderlyingType))
    constexpr explicit Fixint(IntegralType value) noexcept : m_value(value) {}

    template <typename FloatingPointType>
        requires std::is_floating_point_v<FloatingPointType> &&
                 (sizeof(FloatingPointType) == sizeof(UnderlyingType))
    constexpr explicit Fixint(FloatingPointType value) noexcept
        : m_value(std::bit_cast<UnderlyingType>(value)) {
        static_assert(std::numeric_limits<FloatingPointType>::is_iec559,
                      "Fixint only supports IEEE 754 floating point types");
    }

    [[nodiscard]] constexpr static Deserialized<Fixint>
    deserialize(std::span<const std::byte> data) noexcept {

        if (data.size() < sizeof(UnderlyingType)) { // error: not enough data
            return Deserialized(Fixint{}, 0);
        }
        UnderlyingType value{};

        value |= std::to_integer<UnderlyingType>(data[0]);
        value |= std::to_integer<UnderlyingType>(data[1]) << 8;
        value |= std::to_integer<UnderlyingType>(data[2]) << 16;
        value |= std::to_integer<UnderlyingType>(data[3]) << 24;
        if constexpr (std::is_same_v<UnderlyingType, std::uint64_t>) {
            value |= std::to_integer<UnderlyingType>(data[4]) << 32;
            value |= std::to_integer<UnderlyingType>(data[5]) << 40;
            value |= std::to_integer<UnderlyingType>(data[6]) << 48;
            value |= std::to_integer<UnderlyingType>(data[7]) << 56;
        }

        return Deserialized(Fixint{value}, sizeof(UnderlyingType));
    }

    [[nodiscard]] constexpr UnderlyingType value() const noexcept {
        return m_value;
    }

    template <typename T>
        requires std::is_arithmetic_v<T>
    [[nodiscard]] constexpr T as() const noexcept {
        if constexpr (std::is_floating_point_v<T>) {
            static_assert(std::numeric_limits<T>::is_iec559,
                          "Fixint only supports IEEE 754 floating point types");
            if constexpr (sizeof(T) == sizeof m_value) {
                return std::bit_cast<T>(m_value);
            } else if constexpr (sizeof(m_value) == sizeof(float)) {
                return std::bit_cast<float>(m_value);
            } else if constexpr (sizeof(m_value) == sizeof(double)) {
                return std::bit_cast<double>(m_value);
            } else {
                throw std::logic_error("Incompatibility: cannot accurately "
                                       "represent a float on this platform");
            }
        } else {
            return static_cast<T>(m_value);
        }
    }

    constexpr void set_value(UnderlyingType value) noexcept { m_value = value; }

    constexpr std::size_t serialize(std::span<std::byte> buffer) const {
        if (buffer.size() < size()) {
            throw std::runtime_error("Buffer too small to serialize Fixint");
        }

        UnderlyingType value = m_value;
        for (int i = 0; i < sizeof(UnderlyingType); i++) {
            buffer[i] = static_cast<std::byte>(value);
            value >>= 8;
        }
        return sizeof(UnderlyingType);
    }

    [[nodiscard]] static constexpr std::size_t size() noexcept {
        return sizeof(UnderlyingType);
    }

  private:
    UnderlyingType m_value{};
};

using Fixint32 = Fixint<std::uint32_t>;
using Fixint64 = Fixint<std::uint64_t>;

} // namespace proto
