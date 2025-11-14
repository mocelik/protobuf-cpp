#include "TestTypes.h"

#include <protobuf-cpp/Deserialize.h>
#include <protobuf-cpp/Key.h>
#include <protobuf-cpp/Serialize.h>
#include <protobuf-cpp/Varint.h>

#include <gtest/gtest.h>

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

TEST(TrivialStruct, fixint_serialize_deserialize) {
    static_assert(std::is_trivially_copyable_v<test::SingleInt_asFixed>,
                  "DoubleInt should be trivially copyable");

    constexpr test::SingleInt_asFixed original{42};

    auto serialized = proto::serialize(original);
    test::SingleInt_asFixed deserialized =
        proto::deserialize<test::SingleInt_asFixed>(serialized);
    ASSERT_EQ(deserialized, original);
}
