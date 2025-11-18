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
                  "SingleInt_asFixed should be trivially copyable");

    constexpr test::SingleInt_asFixed original{42};

    auto serialized = proto::serialize(original);
    test::SingleInt_asFixed deserialized =
        proto::deserialize<test::SingleInt_asFixed>(serialized);
    ASSERT_EQ(deserialized, original);
}

TEST(TrivialStruct, signed_int_and_float_serialize_deserialize) {
    static_assert(std::is_trivially_copyable_v<test::IntAndFloat_asFixed>,
                  "IntAndFloat_asFixed should be trivially copyable");

    constexpr test::IntAndFloat_asFixed original{-1, 0.5f};

    proto::Fixint32 fixfloat(original.value2);
    auto serialized = proto::serialize(original);
    test::IntAndFloat_asFixed deserialized =
        proto::deserialize<test::IntAndFloat_asFixed>(serialized);
    ASSERT_EQ(deserialized, original);
}
