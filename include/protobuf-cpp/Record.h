#pragma once

#include "Deserialized.h"
#include "Field.h"
#include "Fixint.h"
#include "Key.h"
#include "Varint.h"
#include "WireType.h"
#include "protobuf-cpp/Deserialized.h"

#include <concepts>
#include <cstdint>
#include <ranges>
#include <stdexcept>
#include <type_traits>

namespace proto {

template <class T>
concept Wirable = requires {
    { T::k_wire_type } -> std::convertible_to<WireType>;
    { std::declval<T>().serialize() };
    { T::deserialize(std::declval<std::span<const std::byte>>()) };
};

template <Wirable Type> class Record {
  public:
    constexpr explicit Record(Key key, Type value = Type{}) noexcept
        : m_key(key), m_value(value) {}

    constexpr explicit Record(Field field, Type value = Type{}) noexcept
        : m_key(field, Type::k_wire_type), m_value(value) {}

    [[nodiscard]] static constexpr Deserialized<Record>
    deserialize(std::span<const std::byte> data) {
        auto deserialized_key = Varint::deserialize(data);
        if (deserialized_key.num_bytes_read == 0) {
            return Deserialized<Record>{Record{Field{}, Type{}}, 0};
        }

        auto deserialized_value =
            Type::deserialize(data.subspan(deserialized_key.num_bytes_read));

        return Deserialized<Record>{Record{
                                        Key{deserialized_key.value},
                                        deserialized_value.value,
                                    },
                                    deserialized_key.num_bytes_read +
                                        deserialized_value.num_bytes_read};
    }

    [[nodiscard]] constexpr std::vector<std::byte> serialize() const {
        std::vector<std::byte> buffer;
        buffer.resize(size());
        serialize(buffer);
        return buffer;
    }

    constexpr std::size_t serialize(std::span<std::byte> buffer) const {
        if (buffer.size() < size()) {
            throw std::runtime_error("Buffer too small to serialize Record");
        }

        Varint key_varint{m_key.value()};
        auto num_bytes_written = key_varint.serialize(buffer);
        num_bytes_written +=
            m_value.serialize(buffer | std::views::drop(num_bytes_written));
        return num_bytes_written;
    }

    [[nodiscard]] constexpr Key key() const noexcept { return m_key; }
    [[nodiscard]] constexpr Field field_number() const noexcept {
        return m_key.field_number();
    }
    [[nodiscard]] constexpr WireType wire_type() const noexcept {
        return m_key.wire_type();
    }
    [[nodiscard]] constexpr Type value() const noexcept { return m_value; }

    [[nodiscard]] constexpr std::size_t size() const noexcept {
        return Varint{m_key.value()}.size() + m_value.size();
    }

  private:
    Key m_key;
    Type m_value;
};
} // namespace proto
