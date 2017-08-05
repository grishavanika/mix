#include "precompiled.h"

using namespace mix;
using ::testing::NiceMock;

namespace {

Command MakeINCA(WordValue address, std::size_t index_register = 0)
{
	return Command{48, address, index_register, WordField::FromByte(0)};
}

Command MakeINCX(WordValue address, std::size_t index_register = 0)
{
	return Command{55, address, index_register, WordField::FromByte(0)};
}

Command MakeINCI(std::size_t index, WordValue address, std::size_t index_register = 0)
{
	return Command{48 + index, address, index_register, WordField::FromByte(0)};
}

} // namespace

TEST(INCA, Adds_Value_To_RA)
{
	Computer mix;
	Register ra;
	ra.set_value(2);
	mix.set_ra(ra);

	mix.execute(MakeINCA(40));

	ASSERT_EQ(42, mix.ra().value());
}

TEST(INCA, Adds_Value_To_RA_With_Overflow_As_For_ADD_When_Overflow_Happens)
{
	Computer mix;
	Register ra;
	ra.set_value(int{Word::k_max_abs_value});
	mix.set_ra(ra);

	mix.execute(MakeINCA(1));

	ASSERT_EQ(0, mix.ra().value());
	ASSERT_TRUE(mix.has_overflow());
}

TEST(INCX, Adds_Value_To_RX_With_Overflow_Flag_Set_When_Overflow_Happens)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_ra_set()).Times(0);

	Computer mix{&listener};
	Register rx;
	rx.set_value(int{Word::k_max_abs_value});
	mix.set_rx(rx);

	mix.execute(MakeINCX(11));

	ASSERT_EQ(10, mix.rx().value());
	ASSERT_TRUE(mix.has_overflow());
}

TEST(INCI, Overflow_Is_Not_Possible_For_Index_Registers)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_overflow_flag_set()).Times(0);

	Computer mix{&listener};
	IndexRegister r1;
	r1.set_value(4'095);
	mix.set_ri(1, r1);
	mix.set_ri(2, r1);

	// Add 4'095 value to existing R1 4'095 value taking
	// into account value of R2 (4'095)
	mix.execute(MakeINCI(1, 4'095, 2));

	ASSERT_FALSE(mix.has_overflow());

	Register expected_register;
	expected_register.set_value(3 * 4'095);
	const auto expected_value = expected_register.value(WordField{4, 5});
	ASSERT_EQ(expected_value, mix.ri(1).value());
}

// #TODO: tests for DEC* group
