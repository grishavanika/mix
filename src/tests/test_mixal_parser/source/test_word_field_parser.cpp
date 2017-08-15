#include <mixal/word_field_parser.h>
#include <mixal/expression_parser.h>
#include <mixal/parse_exceptions.h>

#include <gtest/gtest.h>

using namespace mixal;

TEST(WordFieldParser, Empty_String_Is_Valid)
{
	WordFieldParser p;
	p.parse("");
	ASSERT_TRUE(p.empty());
}

TEST(WordFieldParser, Throws_InvalidField_Exception_If_Parentheses_Are_Missed)
{
	WordFieldParser p;
	ASSERT_THROW({
		p.parse("(");
	}, ParseError);

	ASSERT_THROW({
		p.parse(")");
	}, ParseError);
}

TEST(WordFieldParser, Throws_InvalidExpression_Exception_For_Empty_Expression_Inside_Parentheses)
{
	WordFieldParser p;
	ASSERT_THROW({
		p.parse("()");
	}, ParseError);
}

TEST(WordFieldParser, Parses_Sring_As_Expression_Inside_Parentheses)
{
	WordFieldParser field_parser;

	field_parser.parse(" ( -1+5*20//6 ) ");
	ASSERT_FALSE(field_parser.empty());
	ASSERT_TRUE(field_parser.expression());

	ExpressionParser expr_parser;
	expr_parser.parse("-1 + 5  * 20 // 6");
	ASSERT_EQ(expr_parser.expression(), *field_parser.expression());
}

