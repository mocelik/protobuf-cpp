#pragma once

#include "Deserialized.h"
#include "Varint.h"
#include "WireType.h"
#include "protobuf-cpp/Deserialized.h"

#include <cstdint>

namespace proto {

class Record {
  public:
    constexpr explicit Record(std::uint64_t field_number,
                              Varint value = 0) noexcept
        : m_key((field_number << 3) |
                static_cast<std::uint64_t>(WireType::VARINT)),
          m_value(value) {}

    [[nodiscard]] static constexpr Deserialized<Record>
    deserialize(std::span<const std::byte> data) {
        auto deserialized_key = Varint::deserialize(data);
        auto deserialized_value =
            Varint::deserialize(data.subspan(deserialized_key.bytes_read()));

        return Deserialized<Record>{Record{
                                        deserialized_key.value().value() >> 3,
                                        deserialized_value.value(),
                                    },
                                    deserialized_key.bytes_read() +
                                        deserialized_value.bytes_read()};
    }

    [[nodiscard]] constexpr std::vector<std::byte> serialize() const {
        auto serialized_key = m_key.serialize();
        auto serialized_value = m_value.serialize();

        serialized_key.insert(serialized_key.end(), serialized_value.begin(),
                              serialized_value.end());
        return serialized_key;
    }

    [[nodiscard]] constexpr Varint key() const noexcept { return m_key; }
    [[nodiscard]] constexpr Varint field_number() const noexcept {
        return m_key >> 3;
    }
    [[nodiscard]] constexpr WireType wire_type() const noexcept {
        return static_cast<WireType>(m_key.value() & 0x07);
    }
    [[nodiscard]] constexpr Varint value() const noexcept { return m_value; }

  private:
    Varint m_key;
    Varint m_value;
};
} // namespace proto
