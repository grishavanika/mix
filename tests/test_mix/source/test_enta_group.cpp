#include <mix/computer.h>
#include <mix/command_processor.h>
#include <mix/command.h>

#include "computer_listener_mock.h"

#include <gtest/gtest.h>

using namespace mix;
using ::testing::NiceMock;

namespace {
Command MakeENTA(WordValue address, std::size_t index_register = 0)
{
	return Command{48, address, index_register, WordField::FromByte(2)};
}

Command MakeENTI(std::size_t index, WordValue address, std::size_t index_register = 0)
{
	return Command{48 + index, address, index_register, WordField::FromByte(2)};
}

Command MakeENNI(std::size_t index, WordValue address, std::size_t index_register = 0)
{
	return Command{48 + index, address, index_register, WordField::FromByte(3)};
}

} // namespace

TEST(ENTA, Enta_With_Zero_Address_Zeroes_RA)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_ra_set()).Times(2);

	Computer mix{&listener};
	Register ra{-int{Word::k_max_abs_value}};
	mix.set_ra(ra);

	mix.execute(MakeENTA(0));

	ASSERT_EQ(WordValue(Sign::Positive, 0), mix.ra().value());
}

TEST(ENTA, Enta_With_Zero_Address_With_Index_Register_Takes_Index_Register_Value)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_ra_set()).Times(2);

	Computer mix{&listener};
	Register ra{-int{Word::k_max_abs_value}};
	IndexRegister r1{-2000};
	mix.set_ra(ra);
	mix.set_ri(1, r1);

	mix.execute(MakeENTA(0, 1));

	ASSERT_EQ(-2000, mix.ra().value());
}

TEST(ENTA, Enta_With_Zero_Address_With_Index_Register_Takes_Zero_Negative_Index_Value_As_Positive_Zero)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_ra_set()).Times(2);

	Computer mix{&listener};
	Register ra{-int{Word::k_max_abs_value}};
	IndexRegister r1{Sign::Negative, 0, 0};
	mix.set_ra(ra);
	mix.set_ri(1, r1);

	mix.execute(MakeENTA(WordValue{Sign::Positive, 0}, 1));

	ASSERT_EQ(0, mix.ra().value());
}

TEST(ENTA, Enta_With_Negative_Zero_Address_With_Index_Register_Takes_Zero_Index_Value_As_Negative_Zero)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_ra_set()).Times(2);

	Computer mix{&listener};
	Register ra{-int{Word::k_max_abs_value}};
	IndexRegister r1{Sign::Positive, 0, 0};
	mix.set_ra(ra);
	mix.set_ri(1, r1);

	mix.execute(MakeENTA(WordValue{Sign::Negative, 0}, 1));

	ASSERT_EQ(WordValue(Sign::Negative, 0), mix.ra().value());
}

TEST(ENNI, Enter_Negative_I_For_Itself_Changes_Index_Register_Sign)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_ri_set(3)).Times(2);

	Computer mix{&listener};
	IndexRegister r3{+2000};
	mix.set_ri(3, r3);

	mix.execute(MakeENNI(3, 0, 3));

	ASSERT_EQ(-2000, mix.ri(3).value());
}

TEST(ENNI, Enter_Negative_I_For_Itself_Leaves_Negative_Zero_Unchanged)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_ri_set(3)).Times(2);

	Computer mix{&listener};
	IndexRegister r3{Sign::Negative, 0, 0};
	mix.set_ri(3, r3);

	mix.execute(MakeENNI(3, 0, 3));

	ASSERT_EQ(WordValue(Sign::Negative, 0), mix.ri(3).value());
}

TEST(ENTI, Enter_Too_Big_Index_Register_Value_Zeroes_Bytes_Except_Last_2)
{
	Computer mix{};

	IndexRegister r1{-4'095};
	mix.set_ri(1, r1);

	mix.execute(MakeENTI(5, -4'095, 1));

	const auto& r5 = mix.ri(5);
	ASSERT_EQ(Sign::Negative, r5.sign());
	ASSERT_EQ(0, r5.byte(1));
	ASSERT_EQ(0, r5.byte(2));
	ASSERT_EQ(0, r5.byte(3));
	ASSERT_NE(0, r5.byte(4));
	ASSERT_NE(0, r5.byte(5));
}
