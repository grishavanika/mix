#include "precompiled.h"

using namespace mix;
using ::testing::NiceMock;

TEST(CMPA, Comparing_RA_With_Same_Field_Of_Memory_Cell)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_comparison_state_set()).Times(6);

	Computer mix{&listener};
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

