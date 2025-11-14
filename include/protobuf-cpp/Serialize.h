#pragma once

#include "Encoding.h"
#include "Field.h"
#include "Record.h"
#include "Utils.h"
#include "Varint.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace proto {

template <auto MemberPtr, typename T>
void serialize_member_field(const T &obj, std::vector<std::byte> &buffer,
                            Field &field_number) {
    using EncodingType = typename MemberEncoding<MemberPtr>::type;

    EncodingType encoded_obj{obj.*MemberPtr};
    Record record{field_number, encoded_obj};
    auto serialized = serialize(record);
    buffer.insert(buffer.end(), serialized.begin(), serialized.end());
    field_number = Field{std::to_underlying(field_number) + 1};
}

template <typename T> std::vector<std::byte> serialize(const T &obj) {
    std::vector<std::byte> buffer;
    Field field_number{1};

    [&]<auto... MemberPtrs>(Members<MemberPtrs...>) {
        (serialize_member_field<MemberPtrs>(obj, buffer, field_number), ...);
    }(typename T::members{});

    return buffer;
}

} // namespace proto
