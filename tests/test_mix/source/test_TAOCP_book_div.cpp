#include <mix/computer.h>
#include <mix/command.h>

#include <gtest/gtest.h>

using namespace mix;

namespace {
Command MakeMULL(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{4, address, index_register, field};
}

Sign IntSign(int v)
{
	return ((v > 0) ? Sign::Positive : Sign::Negative);
}
} // namespace

TEST(DIV_TAOCP_Book_Test, Div_RAX_With_Cell_Sets_Sign_Of_Result_To_RA)
{
	Computer mix;

	{
		Register ra;
		ra.set_value(WordValue{Sign::Positive, 0});
		mix.set_ra(ra);
	}

	{
		Register rx;
		rx.set_value(17);
		mix.set_rx(rx);
	}

	{
		Word w;
		w.set_value(3);
		mix.set_memory(1000, w);
	}

	const auto ra_sign_before_div = mix.ra().sign();

	mix.execute(MakeMULL(1000));

	ASSERT_EQ(17 / 3, mix.ra().abs_value());
	ASSERT_EQ(17 % 3, mix.rx().abs_value());

	ASSERT_EQ(ra_sign_before_div, mix.rx().sign());
	ASSERT_EQ(IntSign(17 / 3), mix.ra().sign());
}

TEST(DIV_TAOCP_Book_Test, XXXXX)
{
	Computer mix;

	{
		Register ra;
		ra.set_sign(Sign::Negative);
		mix.set_ra(ra);
	}

	{
		Register rx;
		rx.set_value(1235, WordField{0, 2});
		rx.set_byte(3, Byte{0});
		rx.set_byte(4, Byte{3});
		rx.set_byte(5, Byte{1});
		mix.set_rx(rx);
	}

	{
		Word w;
		w.set_sign(Sign::Negative);
		w.set_byte(4, Byte{2});
		mix.set_memory(1000, w);
	}

	const auto ra_sign_before_div = mix.ra().sign();

	mix.execute(MakeMULL(1000));

	ASSERT_EQ(0, mix.ra().value(WordField{1, 1}));
	ASSERT_EQ(617, mix.ra().value(WordField{2, 3}));

	ASSERT_EQ(0, mix.rx().value(WordField{1, 1}));
	ASSERT_EQ(0, mix.rx().value(WordField{2, 2}));
	ASSERT_EQ(0, mix.rx().value(WordField{3, 3}));
	ASSERT_EQ(1, mix.rx().value(WordField{5, 5}));

	ASSERT_EQ(ra_sign_before_div, mix.rx().sign());
	ASSERT_EQ(Sign::Positive, mix.ra().sign());
}

