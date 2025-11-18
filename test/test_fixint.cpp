#include <protobuf-cpp/Fixint.h>
#include <protobuf-cpp/Utils.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <limits>
#include <numbers>
#include <ranges>

static_assert(proto::Fixint32::k_wire_type == proto::WireType::FIXED32);
static_assert(proto::Fixint64::k_wire_type == proto::WireType::FIXED64);

/**
 * Verify that encoding a Fixint only and always uses 4 bytes
 */
TEST(Fixint, serialized_fixint_encoded_size) {
    ASSERT_EQ(proto::Fixint32::size(), sizeof(std::uint32_t));
    ASSERT_EQ(proto::Fixint64::size(), sizeof(std::uint64_t));
}

TEST(Fixint, serialize_deserialize_uint) {
    std::initializer_list<std::uint32_t> test_values32 = {
        0x0000'0000u, 0x0000'00ffu, 0x0000'ff00u,
        0x00ff'0000u, 0xff00'0000u, 0xffff'ffffu};

    for (auto v : test_values32) {
        proto::Fixint32 Fixint_original(v);
        auto serialized = proto::serialize(Fixint_original);
        auto deserialized = proto::Fixint32::deserialize(serialized);
        ASSERT_EQ(deserialized.value.value(), v);
        ASSERT_EQ(deserialized.value.as<std::uint32_t>(), v);
        ASSERT_EQ(deserialized.num_bytes_read, sizeof(std::uint32_t));
    }

    std::initializer_list<std::uint64_t> test_values64 = {
        0x0000'0000'0000'0000u, 0x0000'0000'0000'00ffu, 0x0000'0000'0000'ff00u,
        0x0000'0000'00ff'0000u, 0x0000'0000'ff00'0000u, 0x0000'00ff'0000'0000u,
        0x0000'ff00'0000'0000u, 0x00ff'0000'0000'0000u, 0xff00'0000'0000'0000u,
        0xffff'ffff'ffff'ffffu};

    for (auto v : test_values64) {
        proto::Fixint64 Fixint_original(v);
        auto serialized = proto::serialize(Fixint_original);
        auto deserialized = proto::Fixint64::deserialize(serialized);
        ASSERT_EQ(deserialized.value.value(), v);
        ASSERT_EQ(deserialized.value.as<std::uint64_t>(), v);
        ASSERT_EQ(deserialized.num_bytes_read, sizeof(std::uint64_t));
    }
}

TEST(Fixint, serialize_deserialize_int) {
    std::initializer_list<std::int32_t> test_values32 = {
        0,
        1,
        -1,
        2,
        -2,
        std::numeric_limits<std::int32_t>::min(),
        std::numeric_limits<std::int32_t>::max()};

    // static assert to confirm that there is no undefined behaviour
    static_assert(proto::Fixint32{std::numeric_limits<std::int32_t>::min()}
                      .as<std::int32_t>() ==
                  std::numeric_limits<std::int32_t>::min());
    static_assert(proto::Fixint64{std::numeric_limits<std::int64_t>::min()}
                      .as<std::int64_t>() ==
                  std::numeric_limits<std::int64_t>::min());

    for (auto v : test_values32) {
        proto::Fixint32 Fixint_original(v);
        auto serialized = proto::serialize(Fixint_original);
        auto deserialized = proto::Fixint32::deserialize(serialized);
        ASSERT_EQ(deserialized.value.value(), v);
        ASSERT_EQ(deserialized.value.as<std::int32_t>(), v);
        ASSERT_EQ(deserialized.num_bytes_read, sizeof(std::uint32_t));
    }

    std::initializer_list<std::int64_t> test_values64 = {
        0,
        1,
        -1,
        2,
        -2,
        std::numeric_limits<std::int64_t>::min(),
        std::numeric_limits<std::int64_t>::max()};

    for (auto v : test_values64) {
        proto::Fixint64 Fixint_original(v);
        auto serialized = proto::serialize(Fixint_original);
        auto deserialized = proto::Fixint64::deserialize(serialized);
        ASSERT_EQ(deserialized.value.value(), v);
        ASSERT_EQ(deserialized.value.as<std::int64_t>(), v);
        ASSERT_EQ(deserialized.num_bytes_read, sizeof(std::uint64_t));
    }
}

TEST(Fixint, serialize_deserialize_float) {
    auto test_values = {std::numeric_limits<float>::min(), .0f,
                        std::numeric_limits<float>::max(),
                        std::numbers::pi_v<float>, std::numbers::e_v<float>};

    for (auto v : test_values) {
        proto::Fixint32 Fixint_original(v);
        auto serialized = proto::serialize(Fixint_original);
        auto deserialized = proto::Fixint32::deserialize(serialized);
        ASSERT_FLOAT_EQ(deserialized.value.as<float>(), v);
        ASSERT_FLOAT_EQ(deserialized.num_bytes_read, sizeof(std::uint32_t));
    }
}

TEST(Fixint, serialize_deserialize_double) {
    auto test_values = {std::numeric_limits<double>::min(), .0,
                        std::numeric_limits<double>::max(),
                        std::numbers::pi_v<double>, std::numbers::e_v<double>};

    for (auto v : test_values) {
        proto::Fixint64 Fixint_original(v);
        auto serialized = proto::serialize(Fixint_original);
        auto deserialized = proto::Fixint64::deserialize(serialized);
        ASSERT_DOUBLE_EQ(deserialized.value.as<double>(), v);
        ASSERT_DOUBLE_EQ(deserialized.num_bytes_read, sizeof(std::uint64_t));
    }
}
