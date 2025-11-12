#pragma once

#include "WireType.h"
#include "protobuf-cpp/Varint.h"

#include <cstddef>
#include <ranges>
#include <span>
#include <stdexcept>
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

    [[nodiscard]] constexpr std::vector<std::byte> serialize() const {
        Varint length_varint{m_value.size()};
        std::vector<std::byte> serialized;
        serialized.resize(Varint{m_value.size()}.size() + m_value.size());
        serialize(serialized);
        return serialized;
    }

    constexpr std::size_t serialize(std::span<std::byte> buffer) const {
        if (buffer.size() < size()) {
            throw std::runtime_error("Buffer too small to serialize Varlen");
        }

        Varint length_varint{m_value.size()};
        auto num_bytes_written = length_varint.serialize(buffer);

        auto remaining_buffer = buffer | std::views::drop(num_bytes_written);
        auto end_iter =
            std::copy(m_value.begin(), m_value.end(), remaining_buffer.begin());
        return num_bytes_written +
               std::distance(remaining_buffer.begin(), end_iter);
    }

    [[nodiscard]] constexpr std::span<const std::byte> value() const noexcept {
        return m_value;
    }

    [[nodiscard]] constexpr std::size_t size() const noexcept {
        return Varint{m_value.size()}.size() + m_value.size();
    }

  private:
    std::vector<std::byte> m_value;
};
} // namespace proto
