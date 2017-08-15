#include <mixal/constant_literal_parser.h>
#include <mixal/constant_word_expression_parser.h>
#include <mixal/parse_exceptions.h>

#include <gtest/gtest.h>

using namespace mixal;

TEST(ConstantLiteralParser, Throws_InvalidLiteral_Exception_If_Not_Surrounded_With_Equal_Char)
{
	ConstantLiteralParser p;
	ASSERT_THROW({
		p.parse("1000");
	}, ParseError);
}

TEST(ConstantLiteralParser, Throws_ParseError_Expression_In_Equal_Chars_Is_Empty)
{
	ConstantLiteralParser p;
	ASSERT_THROW({
		p.parse("==");
	}, ParseError);
}

TEST(ConstantLiteralParser, Parse_Expression_Inside_Equal_Chars_Like_W_Expression)
{
	ConstantLiteralParser p;
	p.parse("=11(1:1),77=");

	ConstantWordExpressionParser w_parser;
	w_parser.parse("11(1:1),77");

	ASSERT_EQ(w_parser.expression(), p.expression());
}

TEST(ConstantLiteralParser, Throws_ParseError_Expression_For_Too_Long_Expressions)
{
	std::string expr{"10(1:5)"};
	std::string wexpr;
	for (auto _ : {1, 2, 3, 4, 5})
	{
		(void)_;
		wexpr += expr;
		wexpr += ",";
	}
	wexpr += expr;

	ASSERT_LT(k_max_symbol_length, wexpr.size());
	std::string literal = "=" + wexpr + "=";

	ConstantLiteralParser p;

	ASSERT_THROW({
		p.parse(literal);
	}, ParseError);
}
