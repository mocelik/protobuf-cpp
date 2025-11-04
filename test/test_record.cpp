#include "protobuf-cpp/Fixint.h"
#include "protobuf-cpp/Varint.h"
#include "protobuf-cpp/WireType.h"
#include <protobuf-cpp/Record.h>

#include "gtest/gtest.h"

TEST(Record, construct_record_with_varint_payload) {
    constexpr auto value = 150;
    constexpr auto field = 1;
    constexpr auto key =
        (field << 3) | static_cast<std::uint64_t>(proto::WireType::VARINT);
    proto::Varint payload{value};
    proto::Record record(field, payload);

    ASSERT_EQ(payload.value(), value);

    auto serialized = record.serialize();
    ASSERT_EQ(serialized.size(), 3);
    ASSERT_EQ(serialized[0], std::byte{key});  // key
    ASSERT_EQ(serialized[1], std::byte{0x96}); // byte[0] of 150 varint
    ASSERT_EQ(serialized[2], std::byte{0x01}); // byte[1] of 150 varint

    auto deserialized = proto::Record<proto::Varint>::deserialize(serialized);
    ASSERT_EQ(deserialized.bytes_read(), serialized.size());
    auto deserialized_record = deserialized.value();

    ASSERT_EQ(deserialized_record.key().value(), key);
    ASSERT_EQ(deserialized_record.wire_type(), proto::WireType::VARINT);
    ASSERT_EQ(deserialized_record.field_number().value(), field);
    ASSERT_EQ(deserialized_record.value().value(), value);

    // Re-serializing the same record should yield the same byte sequence
    ASSERT_EQ(deserialized_record.serialize(), serialized);
}

TEST(Record, construct_record_with_fixint32_payload) {
    constexpr auto value = 150;
    constexpr auto field = 1;
    constexpr auto key =
        (field << 3) | static_cast<std::uint64_t>(proto::WireType::FIXED32);

    proto::Fixint<std::uint32_t> payload{value};
    proto::Record record(field, payload);
    ASSERT_EQ(payload.value(), value);

    auto serialized = record.serialize();
    ASSERT_EQ(serialized.size(), 5);
    ASSERT_EQ(serialized[0], std::byte{key});  // key
    ASSERT_EQ(serialized[1], std::byte{0x96}); // byte[0] of 150 fixint
    ASSERT_EQ(serialized[2], std::byte{0x00}); // byte[1] of 150 fixint
    ASSERT_EQ(serialized[3], std::byte{0x00}); // byte[2] of 150 fixint
    ASSERT_EQ(serialized[4], std::byte{0x00}); // byte[3] of 150 fixint

    auto deserialized =
        proto::Record<proto::Fixint<std::uint32_t>>::deserialize(serialized);
    ASSERT_EQ(deserialized.bytes_read(), serialized.size());
    auto deserialized_record = deserialized.value();
    ASSERT_EQ(deserialized_record.key().value(), key);
    ASSERT_EQ(deserialized_record.field_number().value(), field);
    ASSERT_EQ(deserialized_record.wire_type(), proto::WireType::FIXED32);
    ASSERT_EQ(deserialized_record.value().value(), value);
}
