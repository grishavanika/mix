#include <mix/computer.h>
#include <mix/command.h>

#include <gtest/gtest.h>

#include <cassert>

using namespace mix;

namespace {

struct LDParam
{
	int address;
	WordValue value;
	WordField field;

	void setup_data(Computer& mix) const
	{
		Word data;
		data.set_value(value, field);
		mix.set_memory(static_cast<std::size_t>(address), data);
	}

	Command make_lda() const
	{
		return Command{8, address, 0, field};
	}

	Command make_ldx() const
	{
		return Command{15, address, 0, field};
	}

	Command make_ldan() const
	{
		return Command{16, address, 0, field};
	}

	Command make_ldxn() const
	{
		return Command{23, address, 0, field};
	}

	Command make_ldi(std::size_t index) const
	{
		assert((index >= 1) && (index <= 8));
		return Command{8 + index, address, 0, field};
	}

	Command make_ldin(std::size_t index) const
	{
		assert((index >= 1) && (index <= 8));
		return Command{16 + index, address, 0, field};
	}
};

class LDTest :
	public ::testing::TestWithParam<LDParam>
{
protected:
	Computer mix;
};

Sign ReverseSign(Sign sign)
{
	return ((sign == Sign::Negative) ? Sign::Positive : Sign::Negative);
}

} // namespace

// #TODO: LD* commands with non-zero index register
// #TODO: implement zero set behavior of LDI* with 1, 2, 3 bytes field

#define MIX_LDI_UB_IMPLEMENTED	0

TEST_P(LDTest, Content_Of_Source_Address_Field_Replaces_Value_Of_Registers)
{
	const auto& param = GetParam();
	param.setup_data(mix);

	mix.execute(param.make_lda());
	mix.execute(param.make_ldx());

	for (std::size_t index = 1; index <= 6; ++index)
	{
		mix.execute(param.make_ldi(index));
	}

	const int expected_abs_value = std::abs(int(param.value));
	const Sign expected_sign = param.field.includes_sign()
		? param.value.sign()
		: Sign::Positive;

	ASSERT_EQ(expected_abs_value, std::abs(int(mix.ra().value())));
	ASSERT_EQ(expected_sign, mix.ra().sign());

	ASSERT_EQ(expected_sign, mix.rx().sign());
	ASSERT_EQ(expected_abs_value, std::abs(int(mix.rx().value())));

	// Index registers has only 2 bytes.
	// Setting value to 1, 2, 3 indexes is UB
	const bool well_defined_behaviour_for_index_registers = (param.field.bytes_count() <= 2);

	for (std::size_t index = 1; index <= 6; ++index)
	{
		const IndexRegister& ri = mix.ri(index);
		if (well_defined_behaviour_for_index_registers)
		{
			ASSERT_EQ(expected_abs_value, std::abs(int(ri.value())));
			ASSERT_EQ(expected_sign, ri.sign());
		}
		else
		{
			// Sign is the same as it should be
			ASSERT_EQ(expected_sign, ri.sign());
			// ... but 1, 2, 3 bytes are zero
#if (MIX_LDI_UB_IMPLEMENTED)
			ASSERT_EQ(ri.byte(1), Byte{0});
			ASSERT_EQ(ri.byte(2), Byte{0});
			ASSERT_EQ(ri.byte(3), Byte{0});
#endif
		}
	}
}

TEST_P(LDTest, Content_Of_Source_Address_Field_Replaces_Value_Of_Registers_With_Reverse_Sign)
{
	const auto& param = GetParam();
	param.setup_data(mix);

	mix.execute(param.make_ldan());
	mix.execute(param.make_ldxn());

	for (std::size_t index = 1; index <= 6; ++index)
	{
		mix.execute(param.make_ldin(index));
	}

	const int expected_abs_value = std::abs(int(param.value));
	const Sign sign = param.field.includes_sign()
		? param.value.sign()
		: Sign::Positive;
	const Sign expected_sign = ReverseSign(sign);

	ASSERT_EQ(expected_abs_value, std::abs(int(mix.ra().value())));
	ASSERT_EQ(expected_sign, mix.ra().sign());
	
	ASSERT_EQ(expected_abs_value, std::abs(int(mix.rx().value())));
	ASSERT_EQ(expected_sign, mix.rx().sign());

	const bool well_defined_behaviour_for_index_registers = (param.field.bytes_count() <= 2);

	for (std::size_t index = 1; index <= 6; ++index)
	{
		const IndexRegister& ri = mix.ri(index);
		if (well_defined_behaviour_for_index_registers)
		{
			ASSERT_EQ(expected_abs_value, std::abs(int(ri.value())));
			ASSERT_EQ(expected_sign, ri.sign());
		}
		else
		{
			// Sign is the same as it should be
			ASSERT_EQ(expected_sign, ri.sign());
			// ... but 1, 2, 3 bytes are zero
#if (MIX_LDI_UB_IMPLEMENTED)
			ASSERT_EQ(ri.byte(1), Byte{0});
			ASSERT_EQ(ri.byte(2), Byte{0});
			ASSERT_EQ(ri.byte(3), Byte{0});
#endif
		}
	}
}

INSTANTIATE_TEST_CASE_P(Load_With_Default_Field,
	LDTest,
	::testing::Values(
		/*00*/LDParam{1, WordValue{0}, Word::MaxField()},
		/*01*/LDParam{2, WordValue{1}, Word::MaxField()},
		/*02*/LDParam{3, WordValue{42}, Word::MaxField()},
		/*03*/LDParam{4, WordValue{4094}, Word::MaxField()},
		/*04*/LDParam{5, WordValue{4095}, Word::MaxField()},
		/*05*/LDParam{6, WordValue{4096}, Word::MaxField()},
		/*06*/LDParam{7, WordValue{262'142}, Word::MaxField()},
		/*07*/LDParam{8, WordValue{262'143}, Word::MaxField()},
		/*08*/LDParam{9, WordValue{262'144}, Word::MaxField()},
		/*09*/LDParam{10, WordValue{16'777'214}, Word::MaxField()},
		/*10*/LDParam{11, WordValue{16'777'215}, Word::MaxField()},
		/*11*/LDParam{12, WordValue{16'777'216}, Word::MaxField()},
		/*12*/LDParam{13, WordValue{1'073'741'822}, Word::MaxField()},
		/*13*/LDParam{13, WordValue{1'073'741'823}, Word::MaxField()},
		/*14*/LDParam{15, WordValue{-1}, Word::MaxField()},
		/*15*/LDParam{16, WordValue{-42}, Word::MaxField()},
		/*16*/LDParam{17, WordValue{-4094}, Word::MaxField()},
		/*17*/LDParam{18, WordValue{-4095}, Word::MaxField()},
		/*18*/LDParam{19, WordValue{-4096}, Word::MaxField()},
		/*19*/LDParam{20, WordValue{-262'142}, Word::MaxField()},
		/*20*/LDParam{21, WordValue{-262'143}, Word::MaxField()},
		/*21*/LDParam{22, WordValue{-262'144}, Word::MaxField()},
		/*22*/LDParam{23, WordValue{-16'777'214}, Word::MaxField()},
		/*23*/LDParam{24, WordValue{-16'777'215}, Word::MaxField()},
		/*24*/LDParam{25, WordValue{-16'777'216}, Word::MaxField()},
		/*25*/LDParam{13, WordValue{-1'073'741'822}, Word::MaxField()},
		/*26*/LDParam{26, WordValue{-1'073'741'823}, Word::MaxField()}
	));

INSTANTIATE_TEST_CASE_P(Load_With_Partial_Field,
	LDTest,
	::testing::Values(
		/*00*/LDParam{1, WordValue{Sign::Positive, 0}, WordField{0, 0}},
		/*01*/LDParam{2, WordValue{Sign::Negative, 0}, WordField{0, 0}},
		/*02*/LDParam{3, WordValue{63}, WordField{0, 1}},
		/*03*/LDParam{4, WordValue{-63}, WordField{0, 1}},
		/*04*/LDParam{5, WordValue{63}, WordField{1, 1}},
		/*05*/LDParam{6, WordValue{-63}, WordField{1, 1}},
		/*06*/LDParam{7, WordValue{63}, WordField{2, 2}},
		/*07*/LDParam{8, WordValue{-63}, WordField{2, 2}},
		/*08*/LDParam{9, WordValue{63}, WordField{3, 3}},
		/*09*/LDParam{10, WordValue{-63}, WordField{4, 4}},
		/*10*/LDParam{11, WordValue{-63}, WordField{5, 5}},

		/*11*/LDParam{12, WordValue{4'095}, WordField{0, 2}},
		/*12*/LDParam{13, WordValue{-4'095}, WordField{0, 2}},
		/*13*/LDParam{14, WordValue{4'095}, WordField{1, 2}},
		/*14*/LDParam{15, WordValue{-4'095}, WordField{3, 4}},
		/*15*/LDParam{16, WordValue{4'095}, WordField{4, 5}},

		/*16*/LDParam{17, WordValue{262'143}, WordField{0, 3}},
		/*17*/LDParam{18, WordValue{-262'143}, WordField{0, 3}},
		/*18*/LDParam{19, WordValue{262'143}, WordField{1, 3}},
		/*19*/LDParam{20, WordValue{-262'143}, WordField{1, 3}},
		/*20*/LDParam{21, WordValue{262'143}, WordField{2, 4}},
		/*21*/LDParam{22, WordValue{-262'143}, WordField{2, 4}},
		/*22*/LDParam{23, WordValue{262'143}, WordField{3, 5}},
		/*23*/LDParam{24, WordValue{-262'143}, WordField{3, 5}},

		/*24*/LDParam{25, WordValue{16'777'215}, WordField{0, 4}},
		/*25*/LDParam{26, WordValue{-16'777'215}, WordField{0, 4}},
		/*26*/LDParam{27, WordValue{16'777'215}, WordField{1, 4}},
		/*27*/LDParam{28, WordValue{-16'777'215}, WordField{1, 4}},

		/*28*/LDParam{29, WordValue{1'073'741'823}, WordField{1, 5}},
		/*29*/LDParam{30, WordValue{-1'073'741'823}, WordField{1, 5}}
	));

