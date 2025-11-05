#pragma once

#include "WireType.h"
#include "protobuf-cpp/Varint.h"

#include <cstddef>
#include <ranges>
#include <span>
#include <type_traits>
#include <vector>

namespace proto {
class Varlen {
  public:
    static constexpr WireType k_wire_type = WireType::LEN;

    constexpr Varlen() = default;

    constexpr explicit Varlen(std::vector<std::byte> value) noexcept
        : m_value(std::move(value)) {}

    constexpr explicit Varlen(std::span<const std::byte> value) noexcept
        : m_value(value.begin(), value.end()) {}

    template <typename T>
        requires std::is_convertible_v<T, std::byte>
    constexpr explicit Varlen(std::span<T> value) noexcept
        : m_value(value.begin(), value.end()) {}

    [[nodiscard]] constexpr static Deserialized<Varlen>
    deserialize(std::span<const std::byte> data) {
        const auto deserialized_length = Varint::deserialize(data);
        const auto length = deserialized_length.value.value();

        auto remaining_data =
            data | std::views::drop(deserialized_length.num_bytes_read);

        if (remaining_data.size() < length) {
            // Not enough data
            return Deserialized<Varlen>{Varlen{std::vector<std::byte>{}}, 0};
        }

        return Deserialized(Varlen{remaining_data},
                            deserialized_length.num_bytes_read +
                                remaining_data.size());
    }

    [[nodiscard]] constexpr std::vector<std::byte> serialize() const noexcept {
        Varint length_varint{m_value.size()};
        auto serialized = length_varint.serialize();
        serialized.reserve(serialized.size() + m_value.size());
        serialized.insert(serialized.end(), m_value.begin(), m_value.end());
        return serialized;
    }

    [[nodiscard]] constexpr std::span<const std::byte> value() const noexcept {
        return m_value;
    }

  private:
    std::vector<std::byte> m_value;
};
} // namespace proto
