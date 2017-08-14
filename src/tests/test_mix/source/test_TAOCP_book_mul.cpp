#include "precompiled.h"

using namespace mix;

namespace {

Word MakeRandomWord(std::random_device& rd)
{
	std::uniform_int_distribution<std::size_t> dist{0, Byte::k_max_value};
	Word w;
	for (std::size_t i = 1; i <= 5; ++i)
	{
		w.set_byte(i, dist(rd));
	}
	w.set_sign((dist(rd) < (Byte::k_max_value / 2)) ? Sign::Negative : Sign::Positive);
	return w;
}

} // namespace

TEST(MUL_TAOCP_Book_Test, Multiply_RA_With_Cell_Replaces_RA_And_RX_Content)
{
	Computer mix;

	{
		Register ra;
		ra.set_sign(Sign::Positive);
		ra.set_byte(1, 1);
		ra.set_byte(2, 1);
		ra.set_byte(3, 1);
		ra.set_byte(4, 1);
		ra.set_byte(5, 1);
		mix.set_ra(ra);
	}

	{
		Word w;
		w.set_sign(Sign::Positive);
		w.set_byte(1, 1);
		w.set_byte(2, 1);
		w.set_byte(3, 1);
		w.set_byte(4, 1);
		w.set_byte(5, 1);
		mix.set_memory(1000, w);
	}

	mix.execute(MakeMUL(1000));

	Register expected_ra;
	{
		expected_ra.set_sign(Sign::Positive);
		expected_ra.set_byte(1, 0);
		expected_ra.set_byte(2, 1);
		expected_ra.set_byte(3, 2);
		expected_ra.set_byte(4, 3);
		expected_ra.set_byte(5, 4);
	}

	Register expected_rx;
	{
		expected_rx.set_sign(Sign::Positive);
		expected_rx.set_byte(1, 5);
		expected_rx.set_byte(2, 4);
		expected_rx.set_byte(3, 3);
		expected_rx.set_byte(4, 2);
		expected_rx.set_byte(5, 1);
	}

	ASSERT_EQ(expected_ra, mix.ra());
	ASSERT_EQ(expected_rx, mix.rx());
}

TEST(MUL_TAOCP_Book_Test, Multiply_RA_With_Cell_Takes_Into_Account_Field)
{
	std::random_device rd;
	Computer mix;

	Register ra;
	ra.set_value(WordValue{-112});
	Word w = MakeRandomWord(rd);
	w.set_byte(1, 2);
	
	mix.set_ra(ra);
	mix.set_memory(1000, w);

	mix.execute(MakeMUL(1000, WordField{1, 1}));

	ASSERT_EQ(WordValue(Sign::Negative, 0), mix.ra().value());
	ASSERT_EQ(-224, mix.rx().value());
}

TEST(MUL_TAOCP_Book_Test, Multiply_RA_With_Cell_Stores_Most_Significant_Part_In_RA)
{
	Computer mix;

	{
		Register ra;
		ra.set_sign(Sign::Negative);
		ra.set_byte(1, 50);
		ra.set_byte(2, 0);
		ra.set_value(WordValue{112}, WordField{3, 4}, false/*do not touch sign*/);
		ra.set_byte(5, 4);
		mix.set_ra(ra);
	}

	{
		Word w;
		w.set_sign(Sign::Negative);
		w.set_byte(1, 2);
		w.set_byte(2, 0);
		w.set_byte(3, 0);
		w.set_byte(4, 0);
		w.set_byte(5, 0);
		mix.set_memory(1000, w);
	}

	mix.execute(MakeMUL(1000));

	Register expected_ra;
	{
		expected_ra.set_value(WordValue{100}, WordField{0, 2});
		expected_ra.set_byte(3, 0);
		expected_ra.set_value(WordValue{224}, WordField{4, 5});
	}

	Register expected_rx;
	{
		expected_rx.set_sign(Sign::Positive);
		expected_rx.set_byte(1, 8);
		expected_rx.set_byte(2, 0);
		expected_rx.set_byte(3, 0);
		expected_rx.set_byte(4, 0);
		expected_rx.set_byte(5, 0);
	}

	ASSERT_EQ(expected_ra, mix.ra());
	ASSERT_EQ(expected_rx, mix.rx());
}
