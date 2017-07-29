#include <mix/computer.h>
#include <mix/command.h>

#include <gtest/gtest.h>

using namespace mix;

namespace {
Command MakeMULL(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{3, address, index_register, field};
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
