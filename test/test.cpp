#include <gtest/gtest.h>

class ExampleTest : public testing::Test
{
  public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ExampleTest, isFF) { ASSERT_TRUE(1); }
