#include "protobuf-cpp/varint.h"
#include "gtest/gtest.h"
#include <limits>

TEST(Varint, varint_encoded_size) {

    ASSERT_EQ(proto::serialize_varint(0).size(), 1);
    for (int i = 1; i < 10; i++) {
        uint64_t value = (1ULL << (7 * i)) - 1;
        ASSERT_EQ(proto::serialize_varint(value).size(), i);
        ASSERT_EQ(proto::serialize_varint(value + 1).size(), i + 1);
    }
    ASSERT_EQ(proto::serialize_varint(std::numeric_limits<std::uint64_t>::max())
                  .size(),
              10);
}
