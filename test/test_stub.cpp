#include "stub/stub.h"
#include "gtest/gtest.h"

TEST(StubTest, Test1) { EXPECT_TRUE(true); }
TEST(StubTest, add) { EXPECT_EQ(stub_ns::add(2, 3), 5); }
