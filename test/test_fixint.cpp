#include "protobuf-cpp/Fixint.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <array>
#include <limits>
#include <numbers>
#include <ranges>

// Precomputed maximum values for each possible varint size
// equation: max_values[i] = (1 << (7 * (i + 1))) - 1
constexpr std::array<std::uint64_t, 10> max_values = {
    0x0000'0000'0000'007fULL, // 1 byte
    0x0000'0000'0000'3fffULL, // 2 bytes
    0x0000'0000'001f'ffffULL, // 3 bytes
    0x0000'0000'0fff'ffffULL, // 4 bytes
    0x0000'0007'ffff'ffffULL, // 5 bytes
    0x0000'03ff'ffff'ffffULL, // 6 bytes
    0x0001'ffff'ffff'ffffULL, // 7 bytes
    0x00ff'ffff'ffff'ffffULL, // 8 bytes
    0x7fff'ffff'ffff'ffffULL, // 9 bytes
    0xffff'ffff'ffff'ffffULL  // 10 bytes
};
static_assert(max_values[9] == std::numeric_limits<std::uint64_t>::max(),
              "Max value for 10-byte varint should be uint64_t max");

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
        ASSERT_EQ(deserialized.value().value(), v);
        ASSERT_EQ(deserialized.bytes_read(), sizeof(std::uint32_t));
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
        ASSERT_FLOAT_EQ(deserialized.value().value(), v);
        ASSERT_FLOAT_EQ(deserialized.bytes_read(), sizeof(std::uint32_t));
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
        ASSERT_EQ(deserialized.value().value(), v);
        ASSERT_EQ(deserialized.bytes_read(), sizeof(std::uint64_t));
    }
}
