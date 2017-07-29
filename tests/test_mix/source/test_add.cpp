#include <mix/computer.h>
#include <mix/command.h>

#include <gtest/gtest.h>

using namespace mix;

namespace {

Command MakeADD(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{1, address, index_register, field};
}

void SetTestValues(Computer& mix, WordValue ra_value, WordValue memory_value, int address)
{
	Register ra;
	ra.set_value(ra_value);
	mix.set_ra(ra);

	Word memory;
	memory.set_value(memory_value);
	mix.set_memory(address, memory);
}
} // namespace

TEST(Add, When_Overflow_Happens_Overflow_Flag_Is_Set)
{
	Computer mix;
	SetTestValues(mix, Word::k_max_abs_value, 1, 1000);

	mix.execute(MakeADD(1000));

	ASSERT_EQ(0, mix.ra().value());
	ASSERT_TRUE(mix.has_overflow());
}

TEST(Add, When_Overflow_Happens_RA_Is_Set_To_Overflow_Result)
{
	Computer mix;
	SetTestValues(mix, -int{Word::k_max_abs_value}, -2, 1000);

	mix.execute(MakeADD(1000));

	ASSERT_EQ(-1, mix.ra().value());
	ASSERT_TRUE(mix.has_overflow());
}
