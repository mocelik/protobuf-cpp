#include <protobuf-cpp/Fixint.h>
#include <protobuf-cpp/Record.h>
#include <protobuf-cpp/Utils.h>
#include <protobuf-cpp/Varint.h>
#include <protobuf-cpp/Varlen.h>
#include <protobuf-cpp/WireType.h>

#include <gtest/gtest.h>

#include <cstddef>

TEST(Record, construct_record_with_varint_payload) {
    constexpr auto value = 150;
    constexpr proto::Field field{1};
    constexpr auto wire_type = proto::WireType::VARINT;
    constexpr proto::Key key(field, wire_type);
    proto::Varint payload{value};
    proto::Record record(field, payload);

    ASSERT_EQ(payload.value(), value);

    auto serialized = proto::serialize(record);
    ASSERT_EQ(serialized.size(), 3);
    ASSERT_EQ(serialized[0], std::byte(key.value())); // key
    ASSERT_EQ(serialized[1], std::byte{0x96});        // byte[0] of 150 varint
    ASSERT_EQ(serialized[2], std::byte{0x01});        // byte[1] of 150 varint

    auto deserialized = proto::Record<proto::Varint>::deserialize(serialized);
    ASSERT_EQ(deserialized.num_bytes_read, serialized.size());
    auto deserialized_record = deserialized.value;

    ASSERT_EQ(deserialized_record.key(), key);
    ASSERT_EQ(deserialized_record.wire_type(), wire_type);
    ASSERT_EQ(deserialized_record.field_number(), field);
    ASSERT_EQ(deserialized_record.value().value(), value);

    // Re-serializing the same record should yield the same byte sequence
    ASSERT_EQ(proto::serialize(deserialized_record), serialized);
}

TEST(Record, construct_record_with_fixint32_payload) {
    constexpr auto value = 150;
    constexpr proto::Field field{1};
    constexpr auto wire_type = proto::WireType::FIXED32;
    constexpr proto::Key key(field, wire_type);

    proto::Fixint<std::uint32_t> payload{value};
    proto::Record record(field, payload);
    ASSERT_EQ(payload.value(), value);

    auto serialized = proto::serialize(record);
    ASSERT_EQ(serialized.size(), 5);
    ASSERT_EQ(serialized[0], std::byte(key.value())); // key
    ASSERT_EQ(serialized[1], std::byte{0x96});        // byte[0] of 150 fixint
    ASSERT_EQ(serialized[2], std::byte{0x00});        // byte[1] of 150 fixint
    ASSERT_EQ(serialized[3], std::byte{0x00});        // byte[2] of 150 fixint
    ASSERT_EQ(serialized[4], std::byte{0x00});        // byte[3] of 150 fixint

    auto deserialized =
        proto::Record<proto::Fixint<std::uint32_t>>::deserialize(serialized);
    ASSERT_EQ(deserialized.num_bytes_read, serialized.size());
    auto deserialized_record = deserialized.value;
    ASSERT_EQ(deserialized_record.key(), key);
    ASSERT_EQ(deserialized_record.field_number(), field);
    ASSERT_EQ(deserialized_record.wire_type(), wire_type);
    ASSERT_EQ(deserialized_record.value().value(), value);
}

TEST(Record, construct_record_with_fixint64_payload) {
    constexpr auto value = 150;
    constexpr proto::Field field{1};
    constexpr auto wire_type = proto::WireType::FIXED64;
    constexpr proto::Key key(field, wire_type);

    proto::Fixint<std::uint64_t> payload{value};
    proto::Record record(field, payload);
    ASSERT_EQ(payload.value(), value);

    auto serialized = proto::serialize(record);
    ASSERT_EQ(serialized.size(), 9);
    ASSERT_EQ(serialized[0], std::byte(key.value())); // key
    ASSERT_EQ(serialized[1], std::byte{0x96});        // byte[0] of 150 fixint
    ASSERT_EQ(serialized[2], std::byte{0x00});        // byte[1] of 150 fixint
    ASSERT_EQ(serialized[3], std::byte{0x00});        // byte[2] of 150 fixint
    ASSERT_EQ(serialized[4], std::byte{0x00});        // byte[3] of 150 fixint
    ASSERT_EQ(serialized[5], std::byte{0x00});        // byte[4] of 150 fixint
    ASSERT_EQ(serialized[6], std::byte{0x00});        // byte[5] of 150 fixint
    ASSERT_EQ(serialized[7], std::byte{0x00});        // byte[6] of 150 fixint
    ASSERT_EQ(serialized[8], std::byte{0x00});        // byte[7] of 150 fixint

    auto deserialized =
        proto::Record<proto::Fixint<std::uint64_t>>::deserialize(serialized);
    ASSERT_EQ(deserialized.num_bytes_read, serialized.size());
    auto deserialized_record = deserialized.value;
    ASSERT_EQ(deserialized_record.key(), key);
    ASSERT_EQ(deserialized_record.field_number(), field);
    ASSERT_EQ(deserialized_record.wire_type(), proto::WireType::FIXED64);
    ASSERT_EQ(deserialized_record.value().value(), value);
}

TEST(Record, construct_record_with_varlen_payload) {
    std::vector<std::byte> value = {std::byte{0}, std::byte{0xff},
                                    std::byte{0xaa}, std::byte{0x0f}};
    constexpr proto::Field field{1};
    constexpr auto wire_type = proto::WireType::LEN;
    constexpr proto::Key key(field, wire_type);

    proto::Varlen payload(value);
    proto::Record record(field, payload);
    ASSERT_TRUE(std::ranges::equal(payload.value(), value));

    auto serialized = proto::serialize(record);
    ASSERT_EQ(serialized.size(), 6);
    ASSERT_EQ(serialized[0], std::byte(key.value())); // key
    ASSERT_EQ(serialized[1],
              std::byte(value.size())); // varint - length of data - 4
    ASSERT_EQ(serialized[2], value[0]);
    ASSERT_EQ(serialized[3], value[1]);
    ASSERT_EQ(serialized[4], value[2]);
    ASSERT_EQ(serialized[5], value[3]);

    auto deserialized = proto::Record<proto::Varlen>::deserialize(serialized);
    ASSERT_EQ(deserialized.num_bytes_read, serialized.size());
    auto deserialized_record = deserialized.value;
    ASSERT_EQ(deserialized_record.key(), key);
    ASSERT_EQ(deserialized_record.field_number(), field);
    ASSERT_EQ(deserialized_record.wire_type(), proto::WireType::LEN);
    ASSERT_TRUE(std::ranges::equal(deserialized_record.value().value(), value));
}
