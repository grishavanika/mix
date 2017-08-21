#include "precompiled.h"

using namespace mix;

TEST(Byte, Construction_FromNumbers_InRange_DoesNot_Throw)
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

TEST(Byte, Construction_FromNumbers_OutOfRange_Throws_OverflowError)
{
	ASSERT_THROW(
	{
		const int too_big_number = Byte::k_max_value + 42;
		Byte byte{too_big_number};
	}
	, MixException);

	ASSERT_THROW(
	{
		const int too_small_number = Byte::k_min_value - 42;
		Byte byte{too_small_number};
	}
	, MixException);
}

TEST(Byte, CanHold_Values_InRange)
{
	for (auto value = Byte::k_min_value; value <= Byte::k_max_value; ++value)
	{
		ASSERT_TRUE(Byte::CanHoldValue(value));
	}
}

TEST(Byte, CantHold_Values_Under_MinimumPossible_Value)
{
	for (int value = Byte::k_min_value - 42; value < Byte::k_min_value; ++value)
	{
		ASSERT_FALSE(Byte::CanHoldValue(value));
	}
}

TEST(Byte, CantHold_Values_Under_MaximumPossible_Value)
{
	for (int value = Byte::k_max_value + 1; value < Byte::k_max_value + 64; ++value)
	{
		ASSERT_FALSE(Byte::CanHoldValue(value));
	}
}

TEST(Byte, SetValues_InRange_DoesNot_Throw)
{
	Byte byte;
	byte.set(1);
	ASSERT_EQ(byte.cast_to<int>(), 1);
	ASSERT_EQ(byte.value(), 1);

	byte.set(42);
	ASSERT_EQ(byte.cast_to<int>(), 42);
	ASSERT_EQ(byte.value(), 42);

	byte.set(Byte::k_min_value);
	ASSERT_EQ(byte.value(), Byte::k_min_value);

	byte.set(Byte::k_max_value);
	ASSERT_EQ(byte.value(), Byte::k_max_value);
}

TEST(Byte, SetValues_OutOfRange_Throws_OverflowError)
{
	Byte byte;
	byte.set(1);
	ASSERT_EQ(byte.value(), 1);

	ASSERT_THROW(
	{
		byte.set(Byte::k_max_value + 42);
	}
	, ByteOverflow);
	
	ASSERT_EQ(byte.value(), 1);

	ASSERT_THROW(
	{
		byte.set(Byte::k_min_value - 42);
	}
	, ByteOverflow);

	ASSERT_EQ(byte.value(), 1);
}

TEST(Byte, Bytes_WithSameValues_AreEqual)
{
	ASSERT_EQ(Byte{0}, Byte{0});
	ASSERT_EQ(Byte{1}, Byte{1});
	ASSERT_EQ(Byte{32}, Byte{32});
	ASSERT_EQ(Byte::Min(), Byte::Min());
	ASSERT_EQ(Byte::Max(), Byte::Max());
}

TEST(Byte, Bytes_WithDifferentValues_AreDifferent)
{
	ASSERT_NE(Byte{1}, Byte{0});
	ASSERT_NE(Byte{2}, Byte{3});
	ASSERT_NE(Byte{32}, Byte{42});
	ASSERT_NE(Byte::Min(), Byte::Max());
}
