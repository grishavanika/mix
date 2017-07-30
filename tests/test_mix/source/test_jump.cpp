#include <mix/computer.h>
#include <mix/command_processor.h>
#include <mix/command.h>

#include "computer_listener_mock.h"

#include <gtest/gtest.h>

using namespace mix;
using ::testing::_;
using ::testing::NiceMock;

namespace {

Command MakeJMP(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(Byte{0})};
}

Command MakeJSJ(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(Byte{1})};
}

Command MakeJOV(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(Byte{2})};
}

Command MakeJNOV(WordValue address, std::size_t index_register = 0)
{
	return Command{39, address, index_register, WordField::FromByte(Byte{3})};
}

} // namespace

TEST(JMP, Jump_Changes_Current_Command_And_Sets_RJ_To_Next_After_Next_Command)
{
	ComputerListenerMock listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(4000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.execute(MakeJMP(4000));

	ASSERT_EQ(4000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());
}

TEST(JSJ, Jump_Save_RJ_Changes_Current_Command_And_Leaves_Unchanged_RJ)
{
	ComputerListenerMock listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(4000)).Times(1);
	EXPECT_CALL(listener, on_jump(_)).Times(0);

	Computer mix{&listener};
	mix.set_next_command(3000);

	const auto prev_rj = mix.rj();
	ASSERT_NE(3001, prev_rj.value());

	mix.execute(MakeJSJ(4000));

	ASSERT_EQ(4000, mix.current_command());
	ASSERT_EQ(prev_rj, mix.rj());
}

TEST(JOV, Jumps_If_Overflow_Resets_Overflow_Flag)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(4000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.execute(MakeJOV(4000));
	ASSERT_EQ(3000, mix.current_command());

	mix.set_overflow();
	mix.execute(MakeJOV(4000));

	ASSERT_FALSE(mix.has_overflow());
	ASSERT_EQ(4000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());
}

TEST(JNOV, Jumps_If_NO_Overflow)
{
	NiceMock<ComputerListenerMock> listener;
	EXPECT_CALL(listener, on_current_command_changed(_));
	EXPECT_CALL(listener, on_current_command_changed(4000)).Times(1);
	EXPECT_CALL(listener, on_jump(3001)).Times(1);

	Computer mix{&listener};
	mix.set_next_command(3000);

	mix.set_overflow();
	mix.execute(MakeJNOV(4000));
	ASSERT_EQ(3000, mix.current_command());

	mix.clear_overflow();
	mix.execute(MakeJNOV(4000));

	ASSERT_FALSE(mix.has_overflow());
	ASSERT_EQ(4000, mix.current_command());
	ASSERT_EQ(3001, mix.rj().value());
}
