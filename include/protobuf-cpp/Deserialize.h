#pragma once

#include "Field.h"
#include "MemberTuples.h"
#include "Record.h"
#include "Varint.h"
#include "Varlen.h"
#include "protobuf-cpp/Fixint.h"

#include <cstddef>
#include <ranges>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace proto {

template <typename T, typename Tuple, std::size_t... Is>
T make_from_tuple_impl(const Tuple &values, std::index_sequence<Is...>) {
    return T{std::get<Is>(values)...};
}

template <typename T, typename Tuple> T make_from_tuple(const Tuple &values) {
    constexpr auto N = std::tuple_size_v<Tuple>;
    return make_from_tuple_impl<T>(values, std::make_index_sequence<N>{});
}

template <typename T> T deserialize(const std::span<const std::byte> data) {
    decltype(T::members) member_ptrs = T::members;
    member_types_tuple_t<decltype(T::members)> member_types;

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
        if (std::to_underlying(field_number) >
            std::tuple_size_v<decltype(member_ptrs)>) {
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
            set(field_number, deserialized_value.value, member_types);
            break;
        }
        case WireType::FIXED64: {
            auto deserialized_value = Fixint<std::uint64_t>::deserialize(
                data | std::views::drop(deserialized_key.num_bytes_read));
            if (deserialized_value.num_bytes_read == 0) {
                throw std::runtime_error("Error parsing fixint64 for value");
            }
            total_bytes_read += deserialized_value.num_bytes_read;
            set(field_number, deserialized_value.value.value(), member_types);
            break;
        }
        case WireType::LEN: {
            auto deserialized_value = Varlen::deserialize(
                data | std::views::drop(deserialized_key.num_bytes_read));
            if (deserialized_value.num_bytes_read == 0) {
                throw std::runtime_error("Error parsing Varlen for value");
            }
            total_bytes_read += deserialized_value.num_bytes_read;
            // Convert Varlen to a vector of bytes before setting the member
            auto t = deserialized_value.value.value();
            std::vector<std::byte> tmp(t.begin(), t.end());
            set(field_number, tmp, member_types);
            break;
        }

        case WireType::FIXED32: {
            auto deserialized_value = Fixint<std::uint32_t>::deserialize(
                data | std::views::drop(deserialized_key.num_bytes_read));
            if (deserialized_value.num_bytes_read == 0) {
                throw std::runtime_error("Error parsing fixint32 for value");
            }
            total_bytes_read += deserialized_value.num_bytes_read;
            set(field_number, deserialized_value.value.value(), member_types);
            break;
        }
        }
    }

    return make_from_tuple<T>(member_types);
}
} // namespace proto
