#include <mixal/constant_word_expression_parser.h>

#include "parser_test_fixture.h"

using namespace mixal;

namespace {

class ConstantWordExpressionParserTest :
	public ParserTest<ConstantWordExpressionParser>
{
};

} // namespace

TEST_F(ConstantWordExpressionParserTest, When_No_Field_Specified_Behaves_Like_Usual_Expression)
{
	parse("*-3");
	ASSERT_EQ(1u, parser_.expression().tokens.size());
	const auto& token = parser_.expression().tokens[0];

	ExpressionParser expr_parser;
	expr_parser.parse("*-3");

	ASSERT_EQ(expr_parser.expression(), token.expression);
	ASSERT_FALSE(token.field);
}

TEST_F(ConstantWordExpressionParserTest, Field_Can_Be_Specified)
{
	parse("* - 3 (1:3)");
	ASSERT_EQ(1u, parser_.expression().tokens.size());
	const auto& token = parser_.expression().tokens[0];

	ExpressionParser expr_parser;
	expr_parser.parse("*-3");

	WordFieldParser field_parser;
	field_parser.parse("(1:3)");

	ASSERT_EQ(expr_parser.expression(), token.expression);
	ASSERT_TRUE(token.field);
	ASSERT_EQ(field_parser.expression(), token.field);
}

TEST_F(ConstantWordExpressionParserTest, Multiple_Expressions_Can_Be_Specified)
{
	parse("*-3, *-3, *-3, *-3, *-3, *-3");
	ASSERT_EQ(6u, parser_.expression().tokens.size());

	ExpressionParser expr_parser;
	expr_parser.parse("*-3");

	for (const auto& token : parser_.expression().tokens)
	{
		ASSERT_EQ(expr_parser.expression(), token.expression);
	}
}

TEST_F(ConstantWordExpressionParserTest, Parses_Only_Valid_Expr)
{
	parse("*-3,");
	reminder_stream_is(",");
}

