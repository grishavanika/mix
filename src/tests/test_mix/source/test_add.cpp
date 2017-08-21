#include "precompiled.h"

using namespace mix;
using ::testing::NiceMock;

namespace {

Word MaxWord()
{
	return int{Word::k_max_abs_value};
}

Word MinWord()
{
	return -int{Word::k_max_abs_value};
}

Register MaxRegister()
{
	return Register{int{Word::k_max_abs_value}};
}

Register MinRegister()
{
	return Register{-int{Word::k_max_abs_value}};
}

struct AddParam
{
	int expected_value;
	bool should_be_overflow;
	Register ra;
	Word w;
	WordField field;
	int address;
};

std::ostream& operator<<(::std::ostream& os, const AddParam& param)
{
	os << "Expected " << param.expected_value << " value "
		<< (param.should_be_overflow ? "with" : "without") << " overflow. "
		<< "rA: " << param.ra << ". "
		<< "Memory address: " << param.address
		<< " with content: " << param.w << " " << param.field;
	return os;
}

class AddTest :
	public ::testing::TestWithParam<AddParam>
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

TEST(Add, RA_Sign_Is_Not_Changed_If_Sum_Is_Zero)
{
	Computer mix{};
	mix.set_ra(Register{-1});
	mix.set_memory(1000, 1);
	
	mix.execute(MakeADD(1000, Word::MaxField()));

	ASSERT_EQ(0, mix.ra().value());
	ASSERT_EQ(Sign::Negative, mix.ra().sign());
	ASSERT_FALSE(mix.has_overflow());
}

TEST_P(AddTest, Adds_Memory_Cell_To_RA)
{
	const auto& param = GetParam();
	const int overflows_count = param.should_be_overflow ? 1 : 0;
	EXPECT_CALL(listener, on_overflow_flag_set()).Times(overflows_count);

	mix.execute(MakeADD(param.address, param.field));

	ASSERT_EQ(param.expected_value, mix.ra().value());
	ASSERT_EQ(param.should_be_overflow, mix.has_overflow());
}

INSTANTIATE_TEST_CASE_P(Add_With_Overflow,
	AddTest,
	::testing::Values(
		/*00*/AddParam{0, true, MinRegister(), -1, Word::MaxField(), 1000},
		/*01*/AddParam{-1, true, MinRegister(), -2, Word::MaxField(), 1000},
		/*02*/AddParam{0, true, MaxRegister(), 1, Word::MaxField(), 1000},
		/*03*/AddParam{1, true, MaxRegister(), 2, Word::MaxField(), 1000},
		/*04*/AddParam{MaxWord().value() - 1, true, MaxRegister(), MaxWord(), Word::MaxField(), 1000},
		/*05*/AddParam{MinWord().value() + 1, true, MinRegister(), MinWord(), Word::MaxField(), 1000},
		/*06*/AddParam{0, true, MaxRegister(), {1, WordField{5, 5}}, WordField{5, 5}, 1000},
		/*07*/AddParam{1, true, MaxRegister(), {2, WordField{4, 4}}, WordField{4, 4}, 1000}
	));

INSTANTIATE_TEST_CASE_P(Add_Without_Overflow,
	AddTest,
	::testing::Values(
		/*00*/AddParam{0, false, -1, 1, Word::MaxField(), 1000},
		/*01*/AddParam{2, false, 1, 1, Word::MaxField(), 1000},
		/*02*/AddParam{4'096, false, 4'095, 1, Word::MaxField(), 1000},
		/*03*/AddParam{16'777'216, false, 1, 16'777'215, Word::MaxField(), 1000},
		/*04*/AddParam{1 + Byte::k_max_value, false, 1, MaxWord(), WordField{1, 1}, 1000},
		/*05*/AddParam{1 + Byte::k_max_value, false, 1, MaxWord(), WordField{2, 2}, 1000},
		/*06*/AddParam{1 + Byte::k_max_value, false, 1, MaxWord(), WordField{3, 3}, 1000},
		/*07*/AddParam{1 + Byte::k_max_value, false, 1, MaxWord(), WordField{4, 4}, 1000},
		/*08*/AddParam{1 + Byte::k_max_value, false, 1, MaxWord(), WordField{5, 5}, 1000},
		/*09*/AddParam{4'096, false, 1, MaxWord(), WordField{4, 5}, 1000},
		/*10*/AddParam{0, false, -4'095, MaxWord(), WordField{3, 4}, 1000},
		/*11*/AddParam{0, false, MinRegister(), MinWord(), WordField{1, 5}, 1000}
	));

INSTANTIATE_TEST_CASE_P(Add_Zero_Leads_To_No_Changes,
	AddTest,
	::testing::Values(
		/*00*/AddParam{-1, false, -1, 0, Word::MaxField(), 1000},
		/*01*/AddParam{1, false, 0, 1, Word::MaxField(), 1000},
		/*02*/AddParam{2, false, 2, 1, WordField{0, 0}, 1000}
	));
