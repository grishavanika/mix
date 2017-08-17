#include <mixal_parse/field_parser.h>
#include <mixal_parse/expression_parser.h>

#include "parser_test_fixture.h"

using namespace mixal_parse;

namespace {

class FieldParserTest :
	public ParserTest<FieldParser>
{
};

} // namespace

TEST_F(FieldParserTest, Empty_String_Is_Valid)
{
	parse("");
	reminder_stream_is("");
}

TEST_F(FieldParserTest, Fails_If_Parentheses_Are_Openened_But_Not_Closed)
{
	parse_error("(");
}

TEST_F(FieldParserTest, If_Parentheses_Are_Not_Opened_But_Closed_Parses_Empty_String)
{
	parse(")");
	reminder_stream_is(")");
	ASSERT_TRUE(parser_.empty());
}

TEST_F(FieldParserTest, Fails_If_Expression_Is_Empty_Inside_Parentheses)
{
	parse_error("()");
}

TEST_F(FieldParserTest, Parses_Sring_As_Expression_Inside_Parentheses)
{
	parse(" ( -1+5*20//6 ) ");
	ASSERT_FALSE(parser_.empty());
	ASSERT_TRUE(parser_.expression());

	ExpressionParser expr_parser;
	expr_parser.parse_stream("-1 + 5  * 20 // 6");
	ASSERT_EQ(expr_parser.expression(), *parser_.expression());
}

TEST_F(FieldParserTest, Ignores_Rest_Of_Non_Field_Expression)
{
	parse("(1:2),1000");
	ASSERT_TRUE(parser_.expression());
	
	reminder_stream_is(",1000");
}
