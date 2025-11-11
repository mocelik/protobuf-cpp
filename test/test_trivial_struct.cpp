#include "TestTypes.h"
#include "protobuf-cpp/Deserialize.h"
#include "protobuf-cpp/Serialize.h"

#include "gtest/gtest.h"

TEST(TrivialStruct, singleint_serialize_deserialize) {
    static_assert(std::is_trivially_copyable_v<test::SingleInt>,
                  "SingleInt should be trivially copyable");

    constexpr test::SingleInt original{42};

    auto serialized = proto::serialize(original);
    test::SingleInt deserialized =
        proto::deserialize<test::SingleInt>(serialized);
    ASSERT_EQ(deserialized, original);
}

TEST(TrivialStruct, doubleint_serialize_deserialize) {
    static_assert(std::is_trivially_copyable_v<test::DoubleInt>,
                  "DoubleInt should be trivially copyable");

    constexpr test::DoubleInt original{42, 150};

    auto serialized = proto::serialize(original);
    test::DoubleInt deserialized =
        proto::deserialize<test::DoubleInt>(serialized);
    ASSERT_EQ(deserialized, original);
}
