#include <protobuf-cpp/Utils.h>
#include <protobuf-cpp/Varint.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <limits>
#include <ranges>

namespace {
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
static_assert(max_values[9] == std::numeric_limits<std::uint64_t>::max());
} // namespace

static_assert(proto::Varint::k_wire_type == proto::WireType::VARINT);

/**
 * Verify that encoding a varint produces the expected number of bytes.
 */
TEST(Varint, serialized_varint_encoded_size) {

    ASSERT_EQ(proto::Varint{0}.size(), 1);

    std::ranges::for_each(max_values, [](const auto &value) {
        static int num_bytes = 1;
        ASSERT_EQ(proto::Varint{value}.size(), num_bytes);
        num_bytes++;
    });

    std::for_each(max_values.begin(), max_values.end() - 1,
                  [](const auto &value) {
                      static int num_bytes = 2;
                      ASSERT_EQ(proto::Varint{value + 1}.size(), num_bytes);
                      num_bytes++;
                  });
}

/**
 * Verify that encoding a varint produces the expected byte values. There are
 * too many values to check them all, so only the transition points are checked
 */
TEST(Varint, serialized_varint_encoded_value) {

    // Check the 0-case
    {
        auto serialized = proto::serialize(proto::Varint{0});
        ASSERT_EQ(serialized.size(), 1);
        ASSERT_EQ(serialized[0], std::byte{0});
    }

    // Check the transition points
    std::ranges::for_each(max_values, [](const auto value) constexpr {
        auto serialized = proto::serialize(proto::Varint{value});
        std::for_each(
            serialized.begin(), serialized.end() - 1,
            [](const auto &byte) { ASSERT_EQ(byte, std::byte{0b1111'1111}); });

        // Check if the last byte has the continue bit set, except for the max
        // 10-byte varint
        if (serialized.size() < 10) {
            ASSERT_EQ(serialized.back(), std::byte{0b0111'1111});
        } else {
            // The 10th byte of the varint is oversized and won't be completely
            // set
            ASSERT_EQ(serialized.back(), std::byte{1});
        }
    });
}

/**
 * Verify the deserializing a varint
 * This test cases assumes that serializing is already tested and working
 */
TEST(Varint, deserialize_varint) {

    // The maximum values for each varint size
    std::ranges::for_each(max_values, [](const auto &value) {
        auto serialized = proto::serialize(proto::Varint{value});
        auto deserialized = proto::Varint::deserialize(serialized);
        ASSERT_EQ(deserialized.value.value(), value);

        serialized = proto::serialize(proto::Varint{value + 1});
        deserialized = proto::Varint::deserialize(serialized);
        ASSERT_EQ(deserialized.value.value(), value + 1);

        // The 0 case is handled by the highest value wrapping around
    });
}

/**
 * Verify that deserialized value contains correct number of bytes read
 */
TEST(Varint, deserialized_num_bytes_read) {
    for (std::size_t num_bytes = 0; auto v : max_values) {
        auto serialized = proto::serialize(proto::Varint{v});
        auto deserialized = proto::Varint::deserialize(serialized);
        ASSERT_EQ(deserialized.value.value(), v);
        ASSERT_EQ(deserialized.num_bytes_read, serialized.size());
        ASSERT_EQ(deserialized.num_bytes_read, ++num_bytes);
    }
}

TEST(Varint, ZigzagEncoding) {
    ASSERT_EQ(proto::Varint{uint8_t{0}}.as<int8_t>(), 0);
    ASSERT_EQ(proto::Varint{uint8_t{1}}.as<int8_t>(), -1);
    ASSERT_EQ(proto::Varint{uint8_t{2}}.as<int8_t>(), 1);
    ASSERT_EQ(proto::Varint{uint8_t{3}}.as<int8_t>(), -2);
    ASSERT_EQ(proto::Varint{uint8_t{4}}.as<int8_t>(), 2);

    ASSERT_EQ(proto::Varint{int8_t{0}}.as<uint8_t>(), 0);
    ASSERT_EQ(proto::Varint{int8_t{-1}}.as<uint8_t>(), 1);
    ASSERT_EQ(proto::Varint{int8_t{1}}.as<uint8_t>(), 2);
    ASSERT_EQ(proto::Varint{int8_t{-2}}.as<uint8_t>(), 3);
    ASSERT_EQ(proto::Varint{int8_t{2}}.as<uint8_t>(), 4);
    ASSERT_EQ(proto::Varint{int8_t{-3}}.as<uint8_t>(), 5);
    ASSERT_EQ(proto::Varint{int8_t{3}}.as<uint8_t>(), 6);
    ASSERT_EQ(proto::Varint{int8_t{-4}}.as<uint8_t>(), 7);
    ASSERT_EQ(proto::Varint{int8_t{4}}.as<uint8_t>(), 8);

    for (int16_t i = 0; i < 256; i++) {
        ASSERT_EQ(proto::Varint{int16_t{i}}.as<int16_t>(), i);
    }
    for (uint16_t i = 0; i < 256; i++) {
        ASSERT_EQ(proto::Varint{uint16_t{i}}.as<uint16_t>(), i);
    }

    ASSERT_EQ(proto::Varint{std::numeric_limits<std::int64_t>::max()}
                  .as<std::int64_t>(),
              std::numeric_limits<std::int64_t>::max());

    ASSERT_EQ(proto::Varint{std::numeric_limits<std::int64_t>::min()}
                  .as<std::int64_t>(),
              std::numeric_limits<std::int64_t>::min());

    ASSERT_EQ(proto::Varint{std::numeric_limits<std::uint64_t>::max()}
                  .as<std::uint64_t>(),
              std::numeric_limits<std::uint64_t>::max());
}
