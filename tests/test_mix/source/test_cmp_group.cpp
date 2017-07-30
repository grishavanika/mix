#include <mix/computer.h>
#include <mix/command_processor.h>
#include <mix/command.h>

#include "computer_listener_mock.h"

#include <gtest/gtest.h>

using namespace mix;
using ::testing::NiceMock;

namespace {
Command MakeCMPA(int address, WordField field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{56, address, index_register, field};
}

Command MakeCMPX(int address, WordField field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{63, address, index_register, field};
}

Command MakeCMPI(std::size_t index, int address, WordField field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{56 + index, address, index_register, field};
}
} // namespace

TEST(CMPA, Comparing_RA_With_Same_Field_Of_Memory_Cell)
{
	Computer mix;
	Register ra{-int{Word::k_max_abs_value}};
	mix.set_ra(ra);
	Word memory{4'095, WordField{0, 2}};
	mix.set_memory(1000, memory);

	{
		mix.set_comparison_state(ComparisonIndicator::Less);
		mix.execute(MakeCMPA(1000, WordField{1, 2})); // 4'095 == 4'095
		ASSERT_EQ(ComparisonIndicator::Equal, mix.comparison_state());
	}

	{
		mix.set_comparison_state(ComparisonIndicator::Greater);
		mix.execute(MakeCMPA(1000, WordField{0, 2})); // -4'095 < 4'095
		ASSERT_EQ(ComparisonIndicator::Less, mix.comparison_state());
	}

	{
		mix.set_comparison_state(ComparisonIndicator::Less);
		mix.execute(MakeCMPA(1000, WordField{2, 3})); // 4'095 > (4'095, 64]
		ASSERT_EQ(ComparisonIndicator::Greater, mix.comparison_state());
	}
}

TEST(CMPA, Negative_Zero_Is_Equal_To_Positive_Zero)
{
	Computer mix;
	Register ra{-int{Word::k_max_abs_value}};
	mix.set_ra(ra);
	Word memory{4'095, WordField{0, 2}};
	mix.set_memory(1000, memory);

	{
		mix.set_comparison_state(ComparisonIndicator::Less);
		mix.execute(MakeCMPA(1000, WordField{0, 0})); // -0 == +0
		ASSERT_EQ(ComparisonIndicator::Equal, mix.comparison_state());
	}
}

