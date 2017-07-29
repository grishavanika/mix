#include <mix/computer.h>
#include <mix/command.h>

#include <gtest/gtest.h>

#include <random>

using namespace mix;

namespace {
Command MakeMULL(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{3, address, index_register, field};
}

Word MakeRandomWord(std::random_device& rd)
{
	std::uniform_int_distribution<std::size_t> dist{0, Byte::k_max_value};
	Word w;
	for (std::size_t i = 1; i <= 5; ++i)
	{
		w.set_byte(i, Byte{dist(rd)});
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
		ra.set_byte(1, Byte{1});
		ra.set_byte(2, Byte{1});
		ra.set_byte(3, Byte{1});
		ra.set_byte(4, Byte{1});
		ra.set_byte(5, Byte{1});
		mix.set_ra(ra);
	}

	{
		Word w;
		w.set_sign(Sign::Positive);
		w.set_byte(1, Byte{1});
		w.set_byte(2, Byte{1});
		w.set_byte(3, Byte{1});
		w.set_byte(4, Byte{1});
		w.set_byte(5, Byte{1});
		mix.set_memory(1000, w);
	}

	mix.execute(MakeMULL(1000));

	Register expected_ra;
	{
		expected_ra.set_sign(Sign::Positive);
		expected_ra.set_byte(1, Byte{0});
		expected_ra.set_byte(2, Byte{1});
		expected_ra.set_byte(3, Byte{2});
		expected_ra.set_byte(4, Byte{3});
		expected_ra.set_byte(5, Byte{4});
	}

	Register expected_rx;
	{
		expected_rx.set_sign(Sign::Positive);
		expected_rx.set_byte(1, Byte{5});
		expected_rx.set_byte(2, Byte{4});
		expected_rx.set_byte(3, Byte{3});
		expected_rx.set_byte(4, Byte{2});
		expected_rx.set_byte(5, Byte{1});
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
	w.set_byte(1, Byte{2});
	
	mix.set_ra(ra);
	mix.set_memory(1000, w);

	mix.execute(MakeMULL(1000, WordField{1, 1}));

	ASSERT_EQ(WordValue(Sign::Negative, 0), mix.ra().value());
	ASSERT_EQ(-224, mix.rx().value());
}
