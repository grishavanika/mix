#include <mixal_parse/w_value_parser.h>

#include "parser_test_fixture.h"

using namespace mixal_parse;

namespace {

class WValueParserTest :
	public ParserTest<WValueParser>
{
};

} // namespace

TEST_F(WValueParserTest, When_No_Field_Specified_Behaves_Like_Usual_Expression)
{
	parse("*-3");
	ASSERT_EQ(1u, parser_.value().tokens().size());
	const auto& token = parser_.value().tokens()[0];

	ExpressionParser expr_parser;
	expr_parser.parse_stream("*-3");

	ASSERT_EQ(expr_parser.expression(), token.expression);
	ASSERT_FALSE(token.field);
}

TEST_F(WValueParserTest, Field_Can_Be_Specified)
{
	parse("* - 3 (1:3)");
	ASSERT_EQ(1u, parser_.value().tokens().size());
	const auto& token = parser_.value().tokens()[0];

	ExpressionParser expr_parser;
	expr_parser.parse_stream("*-3");

	FieldParser field_parser;
	field_parser.parse_stream("(1:3)");

	ASSERT_EQ(expr_parser.expression(), token.expression);
	ASSERT_TRUE(token.field);
	ASSERT_EQ(field_parser.field().expression(), token.field);
}

TEST_F(WValueParserTest, Multiple_Expressions_Can_Be_Specified)
{
	parse("*-3, *-3, *-3, *-3, *-3, *-3");
	ASSERT_EQ(6u, parser_.value().tokens().size());

	ExpressionParser expr_parser;
	expr_parser.parse_stream("*-3");

	for (const auto& token : parser_.value().tokens())
	{
		ASSERT_EQ(expr_parser.expression(), token.expression);
	}
}

TEST_F(WValueParserTest, Parses_Only_Valid_Expression_Stream_Part)
{
	parse("*-3,");
	reminder_stream_is(",");
}

