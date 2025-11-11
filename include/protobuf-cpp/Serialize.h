#pragma once

#include "Field.h"
#include "Record.h"
#include "Varint.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace proto {

inline std::vector<std::byte> serialize_primitive(Field field,
                                                  const std::uint32_t &obj) {
    proto::Varint varint{obj};
    proto::Record record(field, varint);
    return record.serialize();
}

template <typename T> std::vector<std::byte> serialize(const T &obj) {
    std::vector<std::byte> serialized_data;
    Field field_number{1};

    auto serialize_member = [&obj, &serialized_data,
                             &field_number](auto member_ptr) {
        auto serialized_obj_member_record =
            serialize_primitive(field_number, obj.*member_ptr);

        serialized_data.insert(serialized_data.end(),
                               serialized_obj_member_record.begin(),
                               serialized_obj_member_record.end());
        field_number = Field{std::to_underlying(field_number) + 1};
    };

    std::apply(
        [&serialize_member](auto... member_ptrs) {
            (..., serialize_member(member_ptrs));
        },
        T::members);
    return serialized_data;
}

} // namespace proto
