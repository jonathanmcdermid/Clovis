#include <gtest/gtest.h>

#include "bitboard.hpp"

int main(int argc, char** argv)
{
    clovis::bitboards::init_bitboards();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
