#pragma once

#include "Field.h"
#include "Fixint.h"
#include "Record.h"
#include "Varint.h"
#include "Varlen.h"

#include <cstddef>
#include <ranges>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace proto {

template <auto MemberPtr, typename Obj, typename T>
constexpr void set_if_correct_type(Obj &obj, const T &value);

template <typename Obj, typename T, auto... MemberPtrs>
constexpr void set_field_in_obj(Field field, Obj &obj, const T &value,
                                Members<MemberPtrs...>);

template <typename Obj>
constexpr Obj deserialize(std::span<const std::byte> data) {
    Obj obj;

    std::size_t total_bytes_read = 0;
    while (total_bytes_read < data.size()) {
        auto deserialized_key =
            Varint::deserialize(data | std::views::drop(total_bytes_read));
        if (deserialized_key.num_bytes_read == 0) {
            throw std::runtime_error("Error parsing varint for key");
        }
        total_bytes_read += deserialized_key.num_bytes_read;

        Key key(deserialized_key.value);
        Field field_number = key.field_number();
        // Recall that field number is 1-indexed, so we use > instead of >= here
        if (std::to_underlying(field_number) > Obj::members::s_num_elems) {
            // Unknown field, skip
            continue;
        }

        switch (key.wire_type()) {
        case WireType::VARINT: {
            auto deserialized_value =
                Varint::deserialize(data | std::views::drop(total_bytes_read));
            if (deserialized_value.num_bytes_read == 0) {
                throw std::runtime_error("Error parsing varint for value");
            }
            total_bytes_read += deserialized_value.num_bytes_read;
            set_field_in_obj(field_number, obj, deserialized_value.value,
                             typename Obj::members{});
            break;
        }
        case WireType::FIXED64: {
            auto deserialized_value = Fixint<std::uint64_t>::deserialize(
                data | std::views::drop(deserialized_key.num_bytes_read));
            if (deserialized_value.num_bytes_read == 0) {
                throw std::runtime_error("Error parsing fixint64 for value");
            }
            total_bytes_read += deserialized_value.num_bytes_read;
            set_field_in_obj(field_number, obj, deserialized_value.value,
                             typename Obj::members{});
            break;
        }
        case WireType::LEN: {
            auto deserialized_value = Varlen::deserialize(
                data | std::views::drop(deserialized_key.num_bytes_read));
            if (deserialized_value.num_bytes_read == 0) {
                throw std::runtime_error("Error parsing Varlen for value");
            }
            total_bytes_read += deserialized_value.num_bytes_read;
            set_field_in_obj(field_number, obj, deserialized_value.value,
                             typename Obj::members{});
            break;
        }

        case WireType::FIXED32: {
            auto deserialized_value = Fixint<std::uint32_t>::deserialize(
                data | std::views::drop(deserialized_key.num_bytes_read));
            if (deserialized_value.num_bytes_read == 0) {
                throw std::runtime_error("Error parsing fixint32 for value");
            }
            total_bytes_read += deserialized_value.num_bytes_read;
            set_field_in_obj(field_number, obj, deserialized_value.value,
                             typename Obj::members{});
            break;
        }
        }
    }

    return obj;
}

template <auto MemberPtr, typename Obj, typename T>
constexpr void set_if_correct_type(Obj &obj, const T &value) {
    using MemberType = typename std::remove_cvref_t<decltype(obj.*MemberPtr)>;

    if constexpr (InterpretableAs<T, MemberType>) {
        obj.*MemberPtr = value.template as<MemberType>();
    } else {
        // ABI error - e.g. trying to set a span from a fixint
        throw std::logic_error(
            "Tried to construct a field from an incompatible type");
    }
}

template <typename Obj, typename T, auto... MemberPtrs>
constexpr void set_field_in_obj(Field field, Obj &obj, const T &value,
                                Members<MemberPtrs...>) {

    // convert 1-indexed `field` to 0-index for variadic template index
    const std::size_t idx = std::to_underlying(field) - 1;

    // ABI error
    if (idx >= sizeof...(MemberPtrs)) {
        throw std::logic_error("Trying to set a field that doesn't exist");
    }

    // Call set_if_correct_type on the MemberPtr corresponding to the field
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        ((Is == idx ? (set_if_correct_type<MemberPtrs>(obj, value)) : void{}),
         ...);
    }((std::make_index_sequence<sizeof...(MemberPtrs)>{}));
}

} // namespace proto
