#include "precompiled.h"

using namespace mix;
using ::testing::_;
using ::testing::NiceMock;

TEST(JMP, Jump_Changes_Current_Command_And_Sets_RJ_To_Next_After_Next_Command)
{
	ComputerListenerMock listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(1000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.execute(MakeJMP(1000));

	ASSERT_EQ(1000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());
}

TEST(JSJ, Jump_Save_RJ_Changes_Current_Command_And_Leaves_Unchanged_RJ)
{
	ComputerListenerMock listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(1000)).Times(1);
	EXPECT_CALL(listener, on_jump(_)).Times(0);

	Computer mix{&listener};
	mix.set_next_command(3000);

	const auto prev_rj = mix.rj();
	ASSERT_NE(3001, prev_rj.value());

	mix.execute(MakeJSJ(1000));

	ASSERT_EQ(1000, mix.current_command());
	ASSERT_EQ(prev_rj, mix.rj());
}

TEST(JOV, Jumps_If_Overflow_Resets_Overflow_Flag)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(1000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.execute(MakeJOV(2000));
	ASSERT_EQ(3000, mix.current_command());

	mix.set_overflow();
	mix.execute(MakeJOV(1000));

	ASSERT_FALSE(mix.has_overflow());
	ASSERT_EQ(1000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());
}

TEST(JNOV, Jumps_If_NO_Overflow)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(1000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.set_overflow();
	mix.execute(MakeJNOV(2000));
	ASSERT_EQ(3000, mix.current_command());

	mix.clear_overflow();
	mix.execute(MakeJNOV(1000));

	ASSERT_FALSE(mix.has_overflow());
	ASSERT_EQ(1000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());
}

TEST(JL, Jumps_When_Comparison_Is_Less)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(1000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.set_comparison_state(ComparisonIndicator::Greater);
	mix.execute(MakeJL(1000));
	ASSERT_EQ(3000, mix.current_command());

	mix.set_comparison_state(ComparisonIndicator::Less);
	mix.execute(MakeJL(1000));

	ASSERT_EQ(1000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());
}

TEST(JE, Jumps_When_Comparison_Is_Equal)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(1000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.set_comparison_state(ComparisonIndicator::Greater);
	mix.execute(MakeJE(1000));
	ASSERT_EQ(3000, mix.current_command());

	mix.set_comparison_state(ComparisonIndicator::Equal);
	mix.execute(MakeJE(1000));

	ASSERT_EQ(1000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());
}

TEST(JG, Jumps_When_Comparison_Is_Greater)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(1000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.set_comparison_state(ComparisonIndicator::Equal);
	mix.execute(MakeJG(1000));
	ASSERT_EQ(3000, mix.current_command());

	mix.set_comparison_state(ComparisonIndicator::Greater);
	mix.execute(MakeJG(1000));

	ASSERT_EQ(1000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());
}

TEST(JGE, Jumps_When_Comparison_Is_Greater_Or_Equal)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(1000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);
	EXPECT_CALL(listener, on_current_command_changed(2000)).Times(1);
	EXPECT_CALL(listener, on_jump(1001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.set_comparison_state(ComparisonIndicator::Less);
	mix.execute(MakeJGE(1000));
	ASSERT_EQ(3000, mix.current_command());

	mix.set_comparison_state(ComparisonIndicator::Greater);
	mix.execute(MakeJGE(1000));

	ASSERT_EQ(1000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());

	mix.set_comparison_state(ComparisonIndicator::Equal);
	mix.execute(MakeJGE(2000));

	ASSERT_EQ(2000, mix.current_command());
	ASSERT_EQ(1001, mix.rj().value());
}

TEST(JNE, Jumps_When_Comparison_Is_Not_Equal)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(1000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);
	EXPECT_CALL(listener, on_current_command_changed(2000)).Times(1);
	EXPECT_CALL(listener, on_jump(1001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.set_comparison_state(ComparisonIndicator::Equal);
	mix.execute(MakeJNE(1000));
	ASSERT_EQ(3000, mix.current_command());

	mix.set_comparison_state(ComparisonIndicator::Greater);
	mix.execute(MakeJNE(1000));

	ASSERT_EQ(1000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());

	mix.set_comparison_state(ComparisonIndicator::Less);
	mix.execute(MakeJNE(2000));

	ASSERT_EQ(2000, mix.current_command());
	ASSERT_EQ(1001, mix.rj().value());
}

TEST(JLE, Jumps_When_Comparison_Is_Less_Or_Equal)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(1000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);
	EXPECT_CALL(listener, on_current_command_changed(2000)).Times(1);
	EXPECT_CALL(listener, on_jump(1001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.set_comparison_state(ComparisonIndicator::Greater);
	mix.execute(MakeJLE(1000));
	ASSERT_EQ(3000, mix.current_command());

	mix.set_comparison_state(ComparisonIndicator::Less);
	mix.execute(MakeJLE(1000));

	ASSERT_EQ(1000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());

	mix.set_comparison_state(ComparisonIndicator::Equal);
	mix.execute(MakeJLE(2000));

	ASSERT_EQ(2000, mix.current_command());
	ASSERT_EQ(1001, mix.rj().value());
}



