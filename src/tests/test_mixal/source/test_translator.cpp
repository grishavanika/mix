#include <mixal/translator.h>
#include <mixal/exceptions.h>

#include <mixal_parse/expression_parser.h>

#include <mix/exceptions.h>
#include <mix/char_table.h>

#include <core/utils.h>

#include <gtest/gtest.h>

using namespace mixal;
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
	Word expected_word{Word::BytesArray{0, 8, 24, 15, 4}};
	ASSERT_EQ(' ', mix::ByteToChar(0));
	ASSERT_EQ('H', mix::ByteToChar(8));
	ASSERT_EQ('U', mix::ByteToChar(24));
	ASSERT_EQ('N', mix::ByteToChar(15));
	ASSERT_EQ('D', mix::ByteToChar(4));

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
	(void)mix::CharToByte('"', &converted);
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
	}, mix::TooBigWorldValue);
}


