#include "precompiled.h"

using namespace mix;

namespace {
Sign IntSign(int v)
{
	return ((v > 0) ? Sign::Positive : Sign::Negative);
}
} // namespace

TEST(WordValue, Zero_Value_Has_Positive_Sign)
{
	WordValue v{0};
	ASSERT_EQ(Sign::Positive, v.sign());
	ASSERT_EQ(0, v.value());
}

TEST(WordValue, Reverse_Sign_On_Zero_Makes_Zero_Negative_Value)
{
	WordValue v{0};
	auto reversed = v.reverse_sign();
	ASSERT_EQ(Sign::Negative, reversed.sign());
	ASSERT_EQ(0, reversed.value());
}

TEST(WordValue, Value_Constructor_TakesIntoAccount_Value_Sign)
{
	{
		WordValue v{42};
		ASSERT_EQ(IntSign(42), v.sign());
		ASSERT_EQ(Sign::Positive, v.sign());
		ASSERT_EQ(42, v.value());
	}

	{
		WordValue v{-42};
		ASSERT_EQ(IntSign(-42), v.sign());
		ASSERT_EQ(Sign::Negative, v.sign());
		ASSERT_EQ(-42, v.value());
	}
}

TEST(WordValue, Reverse_Sign_Changes_Sign_Always)
{
	WordValue v{-42};
	auto reversed = v.reverse_sign();
	ASSERT_NE(v.sign(), reversed.sign());
	ASSERT_NE(IntSign(v.value()), IntSign(reversed.value()));
}

TEST(WordValue, Sign_Constructor_Does_Not_Affect_To_Value)
{
	WordValue v{Sign::Positive, -42};
	ASSERT_EQ(Sign::Positive, v.sign());
	ASSERT_EQ(-42, v.value());
}
