#include <mix/computer.h>
#include <mix/command.h>

#include <tuple>

#include <gtest/gtest.h>

#include <cassert>

using namespace mix;

namespace {

Sign ReverseSign(Sign sign)
{
	return ((sign == Sign::Negative) ? Sign::Positive : Sign::Negative);
}

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

	std::tuple<int, Sign> expected_value(bool reverse_sign) const
	{
		const int expected_abs_value = std::abs(int(value));
		const Sign expected_sign = field.includes_sign()
			? value.sign()
			: Sign::Positive;
		const auto sign = reverse_sign ? ReverseSign(expected_sign) : expected_sign;
		return {expected_abs_value, sign};
	}

	bool check_register(const Register& r, std::tuple<int, Sign> expected_value) const
	{
		bool result = true;
		result &= (std::get<0>(expected_value) == std::abs(int(r.value())));
		result &= (std::get<1>(expected_value) == r.value().sign());
		return result;
	}

	bool check_index_register(
		const IndexRegister& ri,
		std::tuple<int, Sign> expected_value,
		const Word& ideal_value) const
	{
		// Index registers has only 2 bytes.
		// Setting value to 1, 2, 3 indexes is UB
		const bool well_defined_behaviour_for_index_registers = (field.bytes_count() <= 2);

		if (well_defined_behaviour_for_index_registers)
		{
			return check_register(ri, expected_value);
		}
		else
		{
			return validate_index_register_UB_state(ri, expected_value, ideal_value);
		}
	}

	bool validate_index_register_UB_state(
		const IndexRegister& ri,
		std::tuple<int, Sign> expected_value,
		const Word& ideal_value) const
	{
		bool result = true;
		// Sign is the same as it should be
		result &= (std::get<1>(expected_value) == ri.sign());
		// ... but 1, 2, 3 bytes are zero
		result &= (ri.byte(1) == Byte{0});
		result &= (ri.byte(2) == Byte{0});
		result &= (ri.byte(3) == Byte{0});

		// ... and value is truncated value of Field(4, 5)
		const int abs_ri_value = std::abs(int(ri.value()));
		const int word_part_value = ideal_value.value(WordField{4, 5});
		result &= (abs_ri_value == word_part_value);
		return result;
	}
};

class LDTest :
	public ::testing::TestWithParam<LDParam>
{
protected:
	Computer mix;
};

} // namespace

// #TODO: LD* commands with non-zero index register

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

	const auto expected_value = param.expected_value(false/*DO not reverse original sign*/);

	ASSERT_TRUE(param.check_register(mix.ra(), expected_value));
	ASSERT_TRUE(param.check_register(mix.rx(), expected_value));

	for (std::size_t index = 1; index <= 6; ++index)
	{
		const IndexRegister& ri = mix.ri(index);
		EXPECT_TRUE(param.check_index_register(ri, expected_value, mix.ra()));
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

	const auto expected_value = param.expected_value(true/*reverse original sign*/);

	ASSERT_TRUE(param.check_register(mix.ra(), expected_value));
	ASSERT_TRUE(param.check_register(mix.rx(), expected_value));

	for (std::size_t index = 1; index <= 6; ++index)
	{
		const IndexRegister& ri = mix.ri(index);
		EXPECT_TRUE(param.check_index_register(ri, expected_value, mix.ra()));
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

