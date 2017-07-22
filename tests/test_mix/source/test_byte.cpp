#include <mix/byte.h>

#include <gtest/gtest.h>

using namespace mix;

TEST(Byte, ConstructionFromNumbersInRangeDoesNotThrow)
{
	{
		Byte default_byte;
		ASSERT_EQ(default_byte.cast_to<int>(), 0);
		ASSERT_EQ(default_byte.value(), 0);
	}

	{
		Byte byte{42};
		ASSERT_EQ(byte.cast_to<int>(), 42);
	}

	{
		Byte byte{Byte::k_min_value};
		ASSERT_EQ(byte.value(), Byte::k_min_value);
	}

	{
		Byte byte{Byte::k_max_value};
		ASSERT_EQ(byte.value(), Byte::k_max_value);
	}
}

TEST(Byte, ConstructionFromNumbersOutOfRangeThrowsOverflowError)
{
	ASSERT_THROW(
	{
		const int too_big_number = Byte::k_max_value + 42;
		Byte byte{too_big_number};
	}
	, std::overflow_error);

	ASSERT_THROW(
	{
		const int too_small_number = Byte::k_min_value - 42;
		Byte byte{too_small_number};
	}
	, std::overflow_error);
}

