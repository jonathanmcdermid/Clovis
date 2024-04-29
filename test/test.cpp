#include <gtest/gtest.h>

class ExampleTest : public testing::Test
{
  public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ExampleTest, EX1) { ASSERT_TRUE(1); }
