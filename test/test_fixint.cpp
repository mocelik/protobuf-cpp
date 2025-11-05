#include "protobuf-cpp/Fixint.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <array>
#include <limits>
#include <numbers>
#include <ranges>

/**
 * Verify that encoding a Fixint only and always uses 4 bytes
 */
TEST(Fixint, serialized_varint_encoded_size) {
    ASSERT_EQ(proto::Fixint<std::uint32_t>(0).serialize().size(),
              sizeof(std::uint32_t));
}

TEST(Fixint, serialize_deserialize_uint32) {
    auto test_values = {0x0000'0000u, 0x0000'00ffu, 0x0000'ff00u,
                        0x00ff'0000u, 0xff00'0000u, 0xffff'ffffu};

    for (auto v : test_values) {
        proto::Fixint Fixint_original(v);
        auto serialized = Fixint_original.serialize();
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
        auto serialized = Fixint_original.serialize();
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
        auto serialized = Fixint_original.serialize();
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
        auto serialized = Fixint_original.serialize();
        auto deserialized =
            proto::Fixint<std::uint64_t>::deserialize(serialized);
        ASSERT_EQ(deserialized.value.value(), v);
        ASSERT_EQ(deserialized.num_bytes_read, sizeof(std::uint64_t));
    }
}
