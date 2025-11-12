#include "protobuf-cpp/Fixint.h"
#include "protobuf-cpp/Utils.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <array>
#include <limits>
#include <numbers>
#include <ranges>

/**
 * Verify that encoding a Fixint only and always uses 4 bytes
 */
TEST(Fixint, serialized_fixint_encoded_size) {
    ASSERT_EQ(proto::Fixint<std::int8_t>::size(), 4);
    ASSERT_EQ(proto::Fixint<std::int16_t>::size(), 4);
    ASSERT_EQ(proto::Fixint<std::int32_t>::size(), 4);
    ASSERT_EQ(proto::Fixint<std::uint8_t>::size(), 4);
    ASSERT_EQ(proto::Fixint<std::uint16_t>::size(), 4);
    ASSERT_EQ(proto::Fixint<std::uint32_t>::size(), 4);
    ASSERT_EQ(proto::Fixint<float>::size(), 4);

    ASSERT_EQ(proto::Fixint<std::int64_t>::size(), 8);
    ASSERT_EQ(proto::Fixint<std::uint64_t>::size(), 8);
    ASSERT_EQ(proto::Fixint<double>::size(), 8);
}

TEST(Fixint, serialize_deserialize_uint32) {
    auto test_values = {0x0000'0000u, 0x0000'00ffu, 0x0000'ff00u,
                        0x00ff'0000u, 0xff00'0000u, 0xffff'ffffu};

    for (auto v : test_values) {
        proto::Fixint Fixint_original(v);
        auto serialized = proto::serialize(Fixint_original);
        auto deserialized =
            proto::Fixint<std::uint32_t>::deserialize(serialized);
        ASSERT_EQ(deserialized.value.value(), v);
        ASSERT_EQ(deserialized.num_bytes_read, sizeof(std::uint32_t));
    }
}

TEST(Fixint, serialize_deserialize_float) {
    auto test_values = {std::numeric_limits<float>::min(), .0f,
                        std::numeric_limits<float>::max(),
                        std::numbers::pi_v<float>, std::numbers::e_v<float>};

    for (auto v : test_values) {
        proto::Fixint Fixint_original(v);
        auto serialized = proto::serialize(Fixint_original);
        auto deserialized = proto::Fixint<float>::deserialize(serialized);
        ASSERT_FLOAT_EQ(deserialized.value.value(), v);
        ASSERT_FLOAT_EQ(deserialized.num_bytes_read, sizeof(std::uint32_t));
    }
}

TEST(Fixint, serialize_deserialize_double) {
    auto test_values = {std::numeric_limits<double>::min(), .0,
                        std::numeric_limits<double>::max(),
                        std::numbers::pi_v<double>, std::numbers::e_v<double>};

    for (auto v : test_values) {
        proto::Fixint Fixint_original(v);
        auto serialized = proto::serialize(Fixint_original);
        auto deserialized = proto::Fixint<double>::deserialize(serialized);
        ASSERT_DOUBLE_EQ(deserialized.value.value(), v);
        ASSERT_DOUBLE_EQ(deserialized.num_bytes_read, sizeof(std::uint64_t));
    }
}

TEST(Fixint, serialize_deserialize_uint64) {
    auto test_values = {0x0000'0000'0000'0000ULL, 0x0000'0000'0000'000fULL,
                        0x0000'0000'0000'00ffULL, 0x0000'0000'0000'0ff0ULL,
                        0x0000'0000'0000'ff00ULL, 0x0000'0000'000f'f000ULL,
                        0x0000'0000'00ff'0000ULL, 0x0000'0000'0ff0'0000ULL,
                        0x0000'0000'ff00'0000ULL, 0x0000'000f'f000'0000ULL,
                        0x0000'00ff'0000'0000ULL, 0x0000'0ff0'0000'0000ULL,
                        0x0000'ff00'0000'0000ULL, 0x000f'f000'0000'0000ULL,
                        0x00ff'0000'0000'0000ULL, 0x0ff0'0000'0000'0000ULL,
                        0xff00'0000'0000'0000ULL, 0xf000'0000'0000'0000ULL,
                        0xffff'ffff'ffff'ffffULL};

    for (auto v : test_values) {
        proto::Fixint Fixint_original(v);
        auto serialized = proto::serialize(Fixint_original);
        auto deserialized =
            proto::Fixint<std::uint64_t>::deserialize(serialized);
        ASSERT_EQ(deserialized.value.value(), v);
        ASSERT_EQ(deserialized.num_bytes_read, sizeof(std::uint64_t));
    }
}
