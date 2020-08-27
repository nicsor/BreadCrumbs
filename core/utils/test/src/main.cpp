/**
 * @file main.cpp
 *
 * @brief Test runner for utils.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
