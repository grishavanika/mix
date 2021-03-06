#include "precompiled.h"

using namespace mix;

TEST(ADD_TAOCP_Book_Test, Register_A_Bytes_Sum)
{
	const auto k_byte{Byte::Max()};

	Computer mix;

	{
		Register ra;
		ra.set_byte(1, k_byte);
		ra.set_byte(2, k_byte);
		ra.set_byte(3, k_byte);
		ra.set_byte(4, k_byte);
		ra.set_byte(5, k_byte);
		mix.set_ra(ra);
	}

	mix.execute(MakeSTA(2000));
	mix.execute(MakeLDA(2000, WordField{5, 5}));

	mix.execute(MakeADD(2000, WordField{4, 4}));
	mix.execute(MakeADD(2000, WordField{3, 3}));
	mix.execute(MakeADD(2000, WordField{2, 2}));
	mix.execute(MakeADD(2000, WordField{1, 1}));

	const auto result = mix.ra().value();
	const auto expected_result = k_byte.value() * 5;
	ASSERT_EQ(expected_result, result);
}

TEST(ADD_TAOCP_Book_Test, Simple_Addition)
{
	Computer mix;

	{
		Register ra;
		ra.set_value(1234, WordField{0, 2});
		ra.set_byte(3, 1);
		ra.set_value(150, WordField{4, 5});
		mix.set_ra(ra);
	}

	{
		Word w;
		w.set_value(100, WordField{0, 2});
		w.set_byte(3, 5);
		w.set_value(50, WordField{4, 5});
		mix.set_memory(1000, w);
	}

	mix.execute(MakeADD(1000));

	{
		ASSERT_EQ(1334, mix.ra().value(WordField{0, 2}));
		ASSERT_EQ(6, mix.ra().value(WordField{3, 3}));
		ASSERT_EQ(200, mix.ra().value(WordField{4, 5}));
	}
}

TEST(SUB_TAOCP_Book_Test, Simple_Substraction)
{
	Computer mix;

	{
		Register ra;
		ra.set_value(WordValue{-1234}, WordField{0, 2});
		ra.set_byte(3, 0);
		ra.set_byte(4, 0);
		ra.set_byte(5, 9);
		mix.set_ra(ra);
	}

	{
		Word w;
		w.set_value(WordValue{-2000}, WordField{0, 2});
		w.set_value(WordValue{150}, WordField{3, 4}, false/*do not touch sign*/);
		w.set_byte(5, 0);
		mix.set_memory(1000, w);
	}

	mix.execute(MakeSUB(1000));

	{
		ASSERT_EQ(766, mix.ra().value(WordField{0, 2}));
		ASSERT_EQ(149, mix.ra().value(WordField{3, 4}));
	}
}

