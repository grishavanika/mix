#include <mix/computer.h>
#include <mix/command_processor.h>
#include <mix/registers.h>
#include <mix/command.h>

#include <gtest/gtest.h>

using namespace mix;

namespace {
Command MakeNUM()
{
	return Command{5, 0, 0, WordField::FromByte(0)};
}

Command MakeCHAR()
{
	return Command{5, 0, 0, WordField::FromByte(1)};
}
} // namespace

TEST(Num, Gets_Decimal_Number_From_RAX_Registers_And_Changes_Absolute_Value_Of_RA)
{
	Computer mix;

	const Register ra{{0, 0, 31, 32, 39}, Sign::Negative};
	mix.set_ra(ra);
	const Register rx{{37, 57, 47, 30, 30}, Sign::Positive};
	mix.set_rx(rx);

	mix.execute(MakeNUM());

	ASSERT_EQ(-12977700, mix.ra().value());
	ASSERT_EQ(rx, mix.rx());
	ASSERT_EQ(ra.sign(), mix.ra().sign());
}

TEST(Char, Converts_RAX_Decimal_Digits_To_Digit_Char_Value_And_Puts_To_RA_RX)
{
	Computer mix;
	
	{
		Register ra{-12977699};
		mix.set_ra(ra);
		const Register rx{{37, 57, 47, 30, 30}, Sign::Positive};
		mix.set_rx(rx);
	}

	mix.execute(MakeCHAR());

	const Register expected_ra{{30, 30, 31, 32, 39}, Sign::Negative};
	const Register expected_rx{{37, 37, 36, 39, 39}, Sign::Positive};

	ASSERT_EQ(expected_ra, mix.ra());
	ASSERT_EQ(expected_rx, mix.rx());
}
