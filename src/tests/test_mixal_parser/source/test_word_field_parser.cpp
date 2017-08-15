#include <mixal/word_field_parser.h>
#include <mixal/expression_parser.h>

#include "parser_test_fixture.h"

using namespace mixal;

namespace {

class WordFieldParserTest :
	public ParserTest<WordFieldParser>
{
};

} // namespace

TEST_F(WordFieldParserTest, Empty_String_Is_Valid)
{
	parse("");
	reminder_stream_is("");
}

TEST_F(WordFieldParserTest, Fails_If_Parentheses_Are_Missed)
{
	parse_error("(");
	parse_error(")");
}

TEST_F(WordFieldParserTest, Fails_If_Expression_Is_Empty_Inside_Parentheses)
{
	parse_error("()");
}

TEST_F(WordFieldParserTest, Parses_Sring_As_Expression_Inside_Parentheses)
{
	parse(" ( -1+5*20//6 ) ");
	ASSERT_FALSE(parser_.empty());
	ASSERT_TRUE(parser_.expression());

	ExpressionParser expr_parser;
	expr_parser.parse_stream("-1 + 5  * 20 // 6");
	ASSERT_EQ(expr_parser.expression(), *parser_.expression());
}

TEST_F(WordFieldParserTest, Ignores_Rest_Of_Non_Field_Expression)
{
	parse("(1:2),1000");
	ASSERT_TRUE(parser_.expression());
	
	reminder_stream_is(",1000");
}
