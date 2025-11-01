#include "protobuf-cpp/varint.h"
#include "gtest/gtest.h"

TEST(Varint, Test1) { EXPECT_FALSE(proto::serialize_varint(1).empty()); }
