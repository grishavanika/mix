#include <mixal/translator.h>
#include <mixal/exceptions.h>

#include <mixal_parse/expression_parser.h>
#include <mixal_parse/w_value_parser.h>

#include <mix/exceptions.h>
#include <mix/char_table.h>
#include <mix/computer.h>

#include <core/utils.h>

#include <gtest_all.h>

#include "commands_factory.h"

using namespace mixal;
using namespace mix;
using namespace mixal_parse;

namespace {

class ExpressionEvaluateTest :
	public ::testing::Test
{
protected:
	void expression_is(Word value, const char* expr_str)
	{
		ExpressionParser parser;
		const auto pos = parser.parse_stream(expr_str);
		ASSERT_FALSE(IsInvalidStreamPosition(pos))
			<< "Failed to parse `" << expr_str << "` expression";
		
		ASSERT_EQ(value, translator_.evaluate(parser.expression()));
	}

	void wvalue_is(Word value, const char* expr_str)
	{
		WValueParser parser;
		const auto pos = parser.parse_stream(expr_str);
		ASSERT_FALSE(IsInvalidStreamPosition(pos))
			<< "Failed to parse `" << expr_str << "` W-value";

		ASSERT_EQ(value, translator_.evaluate(parser.value()));
	}

protected:
	Translator translator_;
};

} // namespace

TEST(TranslatorTest, Evaluates_Number_To_Word_With_Same_Integer_Value)
{
	Translator t;
	ASSERT_EQ(111111, t.evaluate(Number{"111111"}));
}

TEST(TranslatorTest, Throws_TooBigWordValueError_When_Number_Can_Not_Be_Hold_By_Word)
{
	Translator t;

	ASSERT_THROW({
		// Note: maximum possible MIXAL number: 10 digits, all 9
		t.evaluate(Number{"9999999999"});
	}, TooBigWordValueError);
}

TEST(TranslatorTest, Evaluates_Text_To_Word_Bytes_That_Corresponds_To_MIX_Char_Table)
{
	Translator t;
	Word expected_word{Word::BytesArray{{0, 8, 24, 15, 4}}};
	ASSERT_EQ(' ', ByteToChar(0));
	ASSERT_EQ('H', ByteToChar(8));
	ASSERT_EQ('U', ByteToChar(24));
	ASSERT_EQ('N', ByteToChar(15));
	ASSERT_EQ('D', ByteToChar(4));

	ASSERT_EQ(expected_word, t.evaluate(Text{" HUND"}));
}


TEST(TranslatorTest, Throws_InvalidALFText_For_Text_With_Wrong_Length)
{
	const char k_short_text[] = "HH";
	ASSERT_LT(core::ArraySize(k_short_text), Word::k_bytes_count);

	Translator t;
	ASSERT_THROW({
		t.evaluate(Text{k_short_text});
	}, InvalidALFText);
}

TEST(TranslatorTest, Throws_InvalidALFText_For_Text_With_Invalid_MIX_Char)
{
	bool converted = true;
	(void)CharToByte('"', &converted);
	ASSERT_FALSE(converted);

	Translator t;
	ASSERT_THROW({
		t.evaluate(Text{R"(""""")"});
	}, InvalidALFText);
}

TEST_F(ExpressionEvaluateTest, Number_Expression_Evaluates_To_Number)
{
	expression_is(42, "42");
}

TEST_F(ExpressionEvaluateTest, Unary_Minus_Negates_Number)
{
	expression_is(-42, "-42");
}

TEST_F(ExpressionEvaluateTest, Unary_Plus_Does_Nothing)
{
	translator_.define_symbol("X", -7);

	expression_is(-7, "+X");
}

TEST_F(ExpressionEvaluateTest, Too_Big_Binary_Plus_Result_Throws_TooBigWorldValue)
{
	translator_.define_symbol("X", int{Word::k_max_abs_value});
	translator_.define_symbol("Y", 1);

	ASSERT_THROW({
		expression_is({}, "X + Y");
	}, TooBigWorldValue);
}

TEST_F(ExpressionEvaluateTest, Binary_Plus_Result_Is_The_Same_As_Knuth_Specification)
{
	Computer computer;
	const int AA = 100;
	const int BB = 1000;
	const int CC = 2000;

	const Command k_add_code[] =
	{
		MakeLDA(AA),
		MakeADD(BB),
		MakeSTA(CC),
	};
	computer.set_memory(AA, -1);
	computer.set_memory(BB, 5);

	for (const auto& command : k_add_code)
	{
		computer.execute(command);
	}

	expression_is(computer.memory(CC), "-1 + 5");
}

TEST_F(ExpressionEvaluateTest, Binary_Minus_Result_Is_The_Same_As_Knuth_Specification)
{
	Computer computer;
	const int AA = 100;
	const int BB = 1000;
	const int CC = 2000;

	const Command k_sub_code[] =
	{
		MakeLDA(AA),
		MakeSUB(BB),
		MakeSTA(CC),
	};
	computer.set_memory(AA, 100);
	computer.set_memory(BB, 3);

	for (const auto& command : k_sub_code)
	{
		computer.execute(command);
	}

	translator_.set_current_address(100);
	expression_is(computer.memory(CC), "* - 3");
}

TEST_F(ExpressionEvaluateTest, Binary_Multiply_Result_Is_The_Same_As_Knuth_Specification)
{
	Computer computer;
	const int AA = 100;
	const int BB = 1000;
	const int CC = 2000;

	const Command k_mul_code[] =
	{
		MakeLDA(AA),
		MakeMUL(BB),
		MakeSTX(CC),
	};
	computer.set_memory(AA, 10);
	computer.set_memory(BB, 10);

	for (const auto& command : k_mul_code)
	{
		computer.execute(command);
	}

	translator_.set_current_address(10);
	expression_is(computer.memory(CC), "***");
}

TEST_F(ExpressionEvaluateTest, Binary_Divide_Result_Is_The_Same_As_Knuth_Specification)
{
	Computer computer;
	const int AA = 100;
	const int BB = 1000;
	const int CC = 2000;

	const Command k_mul_code[] =
	{
		MakeLDA(AA),
		MakeSRAX(5),
		MakeDIV(BB),
		MakeSTA(CC),
	};
	computer.set_memory(AA, -42);
	computer.set_memory(BB, 3);

	for (const auto& command : k_mul_code)
	{
		computer.execute(command);
	}

	expression_is(computer.memory(CC), "-42 / 3");
}

TEST_F(ExpressionEvaluateTest, Binary_DoubleDivide_Result_Is_The_Same_As_Knuth_Specification)
{
	Computer computer;
	const int AA = 100;
	const int BB = 1000;
	const int CC = 2000;

	const Command k_mul_code[] =
	{
		MakeLDA(AA),
		MakeENTX(0),
		MakeDIV(BB),
		MakeSTA(CC),
	};
	computer.set_memory(AA, 1);
	computer.set_memory(BB, 3);

	for (const auto& command : k_mul_code)
	{
		computer.execute(command);
	}

	expression_is(computer.memory(CC), "1 // 3");
}

TEST_F(ExpressionEvaluateTest, Binary_Field_Result_Is_The_Same_As_Knuth_Specification)
{
	Computer computer;
	const int AA = 100;
	const int BB = 1000;
	const int CC = 2000;
	const int KK = 700;

	const Command k_mul_code[] =
	{
		MakeLDA(AA),
		MakeMUL(KK), // =8=
		MakeSLAX(5),
		MakeADD(BB),
		MakeSTA(CC),
	};
	computer.set_memory(AA, 1);
	computer.set_memory(BB, 3);
	computer.set_memory(KK, 8);

	for (const auto& command : k_mul_code)
	{
		computer.execute(command);
	}

	expression_is(computer.memory(CC), "1 : 3");
}

TEST_F(ExpressionEvaluateTest, Operations_Are_Evaluated_From_Left_To_Right_Without_Priorities)
{
	expression_is(13, "-1 + 5 * 20 / 6");
}

TEST_F(ExpressionEvaluateTest, Negative_Zero_Is_Unchanged_If_Second_Operand_Is_Zero_For_Plus_And_Minus)
{
	translator_.define_symbol("Y", WordValue{Sign::Negative, 0});

	expression_is(WordValue{Sign::Negative, 0}, "-0 + 0"); // -0 +  0
	expression_is(WordValue{Sign::Negative, 0}, "-0 + Y"); // -0 + -0
	expression_is(WordValue{Sign::Negative, 0}, "-0 - 0"); // -0 -  0
	expression_is(WordValue{Sign::Negative, 0}, "-0 - Y"); // -0 - -0
}

TEST_F(ExpressionEvaluateTest, Multiply_By_Zero_Is_Always_Positive_Zero)
{
	translator_.define_symbol("Y", WordValue{Sign::Negative, 0});

	expression_is(0, "-0 * 0"); // -0 *  0
	expression_is(0, "-0 * Y"); // -0 * -0
}

TEST_F(ExpressionEvaluateTest, Division_By_Zero_Throws_DivisionByZero)
{
	translator_.define_symbol("Y", WordValue{Sign::Negative, 0});

	ASSERT_THROW({
		expression_is({}, "1 / 0");
	}, DivisionByZero);
	ASSERT_THROW({
		expression_is({}, "1 / Y");
	}, DivisionByZero);
	ASSERT_THROW({
		expression_is({}, "1 // 0");
	}, DivisionByZero);
	ASSERT_THROW({
		expression_is({}, "1 // Y");
	}, DivisionByZero);
}

TEST_F(ExpressionEvaluateTest, WValue_With_Single_Expression_Is_The_Same_Expression)
{
	wvalue_is(1, "1");
}

TEST_F(ExpressionEvaluateTest, WValue_With_Comma_Accumulates_Value_With_Field_To_Final_Value)
{
	Word result;
	result.set_value(-1000, WordField{0, 2});
	result.set_value(1, WordField{3, 5}, false/*do not touch sign*/);

	wvalue_is(result, "1,-1000(0:2)");
}

TEST_F(ExpressionEvaluateTest, WValue_Lates_Field_With_Maximum_Value_Overwrites_Previous_Result)
{
	wvalue_is(1, "-1000(0:2),1");
}

TEST_F(ExpressionEvaluateTest, WValue_With_Too_Big_Field_For_Single_Word_Throws_InvalidWValueField)
{
	ASSERT_THROW({
		wvalue_is({}, "1 (1:42)");
	}, InvalidWValueField);

	ASSERT_THROW({
		wvalue_is({}, "1 (6:7)");
	}, InvalidWValueField);
}


