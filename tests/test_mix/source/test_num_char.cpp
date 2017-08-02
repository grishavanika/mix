#include <mix/computer.h>
#include <mix/command_processor.h>
#include <mix/registers.h>
#include <mix/command.h>

#include <gtest/gtest.h>

using namespace mix;

namespace {
Command MakeNUM()
{
	return Command{5, 0, 0, WordField{0, 0}};
}
} // namespace

TEST(Num, Gets_Decimal_Number_From_RAX_Registers_And_Changes_Absolute_Value_Of_RA)
{
	Computer mix;
	const Register ra{{Byte{0}, Byte{0}, Byte{31}, Byte{32}, Byte{39}}, Sign::Negative};
	mix.set_ra(ra);
	const Register rx{{Byte{37}, Byte{57}, Byte{47}, Byte{30}, Byte{30}}, Sign::Positive};
	mix.set_rx(rx);

	mix.execute(MakeNUM());

	ASSERT_EQ(-12977700, mix.ra().value());
	ASSERT_EQ(rx, mix.rx());
	ASSERT_EQ(ra.sign(), mix.ra().sign());
}

