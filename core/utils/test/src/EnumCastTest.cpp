/**
 * @file EnumCastTest.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <string>

#include <gtest/gtest.h>

#include <core/util/EnumCast.hpp>

enum struct Number
{
    ONE,
    TWO
};

const core::util::EnumCast<Number> number_map =
		core::util::EnumCast<Number>
        (Number::ONE, "1")
        (Number::TWO, "2");

TEST(SuperTest, EnumToString)
{
    EXPECT_EQ("1", number_map(Number::ONE));
    EXPECT_EQ("2", number_map(Number::TWO));
}

TEST(SuperTest, StringToEnum)
{
    EXPECT_EQ(Number::ONE, number_map("1"));
    EXPECT_EQ(Number::TWO, number_map("2"));
}
