#include <mix/computer.h>
#include <mix/command.h>

#include "computer_listener_mock.h"

#include <gtest/gtest.h>

using namespace mix;
using ::testing::NiceMock;

namespace {

Command MakeSUB(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{2, address, index_register, field};
}

Word MaxWord()
{
	return Word{int{Word::k_max_abs_value}};
}

Word MinWord()
{
	return Word{-int{Word::k_max_abs_value}};
}

Register MaxRegister()
{
	return Register{int{Word::k_max_abs_value}};
}

Register MinRegister()
{
	return Register{-int{Word::k_max_abs_value}};
}

struct SubParam
{
	int expected_value;
	bool should_be_overflow;
	Register ra;
	Word w;
	WordField field;
	int address;
};

class SubTest :
	public ::testing::TestWithParam<SubParam>
{
	void SetUp() override
	{
		const auto& param = GetParam();
		mix.set_ra(param.ra);
		mix.set_memory(param.address, param.w);
	}

protected:
	NiceMock<ComputerListenerMock> listener;
	Computer mix{&listener};
};

} // namespace

TEST(Sub, RA_Sign_Is_Not_Changed_If_Sum_Is_Zero)
{
	Computer mix{};
	mix.set_ra(Register{-1});
	mix.set_memory(1000, Word{-1});
	
	mix.execute(MakeSUB(1000, Word::MaxField()));

	ASSERT_EQ(0, mix.ra().value());
	ASSERT_EQ(Sign::Negative, mix.ra().sign());
	ASSERT_FALSE(mix.has_overflow());
}

TEST_P(SubTest, Substracts_Memory_Cell_From_RA)
{
	const auto& param = GetParam();
	const int overflows_count = param.should_be_overflow ? 1 : 0;
	EXPECT_CALL(listener, on_overflow_flag_set()).Times(overflows_count);

	mix.execute(MakeSUB(param.address, param.field));

	ASSERT_EQ(param.expected_value, mix.ra().value());
	ASSERT_EQ(param.should_be_overflow, mix.has_overflow());
}

INSTANTIATE_TEST_CASE_P(Sub_With_Overflow,
	SubTest,
	::testing::Values(
		/*00*/SubParam{0, true, MinRegister(), Word{1}, Word::MaxField(), 1000},
		/*01*/SubParam{-1, true, MinRegister(), Word{2}, Word::MaxField(), 1000},
		/*02*/SubParam{0, true, MaxRegister(), Word{-1}, Word::MaxField(), 1000},
		/*03*/SubParam{1, true, MaxRegister(), Word{-2}, Word::MaxField(), 1000},
		/*04*/SubParam{MaxWord().value() - 1, true, MaxRegister(), MinWord(), Word::MaxField(), 1000},
		/*05*/SubParam{MinWord().value() + 1, true, MinRegister(), MaxWord(), Word::MaxField(), 1000},
		/*06*/SubParam{0, true, MinRegister(), Word{1, WordField{5, 5}}, WordField{2, 2}, 1000}
	));

INSTANTIATE_TEST_CASE_P(Sub_Without_Overflow,
	SubTest,
	::testing::Values(
		/*00*/SubParam{0, false, Register{1}, Word{1}, Word::MaxField(), 1000},
		/*01*/SubParam{0, false, Register{-1}, Word{-1}, Word::MaxField(), 1000},
		/*02*/SubParam{-2, false, Register{-1}, Word{1}, Word::MaxField(), 1000},
		/*03*/SubParam{0, false, Register{int(Byte::k_max_value)}, MaxWord(), WordField{1, 1}, 1000},
		/*04*/SubParam{-4'095, false, Register{0}, MaxWord(), WordField{1, 2}, 1000},
		/*05*/SubParam{0, false, Register{4'095}, MaxWord(), WordField{2, 3}, 1000},
		/*06*/SubParam{-2 * 4'095, false, Register{-4'095}, MaxWord(), WordField{2, 3}, 1000}
		));
