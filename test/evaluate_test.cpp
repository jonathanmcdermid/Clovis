#include <gtest/gtest.h>

#include "src/evaluate.h"

using namespace clovis;
using namespace clovis::eval;

class EvaluateTest : public testing::Test
{
  public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(EvaluateTest, EX1) { 
    ASSERT_FALSE(is_doubled_pawn(0ULL, A1));
}
