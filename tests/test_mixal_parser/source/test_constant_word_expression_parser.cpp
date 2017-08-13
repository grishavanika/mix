#include <mixal/constant_word_expression_parser.h>
#include <mixal/expression_parser.h>
#include <mixal/word_field_parser.h>
#include <mixal/parse_exceptions.h>

#include <gtest/gtest.h>

using namespace mixal;

TEST(ConstantWordExpressionParser, When_No_Field_Specified_Behaves_Like_Usual_Expression)
{
	ConstantWordExpressionParser p;
	p.parse("*-3");
	ASSERT_EQ(1u, p.expression().tokens.size());
	const auto& token = p.expression().tokens[0];

	ExpressionParser expr_parser;
	expr_parser.parse("*-3");

	ASSERT_EQ(expr_parser.expression(), token.expression);
	ASSERT_FALSE(token.field);
}

TEST(ConstantWordExpressionParser, Field_Can_Be_Specified)
{
	ConstantWordExpressionParser p;
	p.parse("* - 3 (1:3)");
	ASSERT_EQ(1u, p.expression().tokens.size());
	const auto& token = p.expression().tokens[0];

	ExpressionParser expr_parser;
	expr_parser.parse("*-3");

	WordFieldParser field_parser;
	field_parser.parse("(1:3)");

	ASSERT_EQ(expr_parser.expression(), token.expression);
	ASSERT_TRUE(token.field);
	ASSERT_EQ(field_parser.expression(), token.field);
}

TEST(ConstantWordExpressionParser, Multiple_Expressions_Can_Be_Specified)
{
	ConstantWordExpressionParser p;
	p.parse("*-3, *-3, *-3, *-3, *-3, *-3");
	ASSERT_EQ(6u, p.expression().tokens.size());

	ExpressionParser expr_parser;
	expr_parser.parse("*-3");

	for (const auto& token : p.expression().tokens)
	{
		ASSERT_EQ(expr_parser.expression(), token.expression);
	}
}


