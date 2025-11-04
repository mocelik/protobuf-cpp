#include "protobuf-cpp/Varint.h"
#include <protobuf-cpp/Record.h>

#include "gtest/gtest.h"

TEST(Record, construct_record_with_varint_payload) {
    proto::Varint payload{150};
    proto::Record<proto::Varint> record(1, payload);

    ASSERT_EQ(payload.value(), 150);

    auto serialized = record.serialize();
    ASSERT_EQ(serialized.size(), 3);
    ASSERT_EQ(serialized[0], std::byte{0x08}); // key
    ASSERT_EQ(serialized[1], std::byte{0x96}); // payload
    ASSERT_EQ(serialized[2], std::byte{0x01}); // payload

    auto deserialized = proto::Record<proto::Varint>::deserialize(serialized);
    ASSERT_EQ(deserialized.bytes_read(), serialized.size());
    auto deserialized_record = deserialized.value();

    ASSERT_EQ(deserialized_record.key().value(), 8);
    ASSERT_EQ(deserialized_record.wire_type(), proto::WireType::VARINT);
    ASSERT_EQ(deserialized_record.field_number().value(), 1);
    ASSERT_EQ(deserialized_record.value().value(), 150);

    // Re-serializing the same record should yield the same byte sequence
    ASSERT_EQ(deserialized_record.serialize(), serialized);
}
