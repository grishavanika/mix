#include <mixal_parse/address_parser.h>

#include "parser_test_fixture.h"
#include "expression_builders.h"

using namespace mixal_parse;

namespace {

class AddressParserTest :
	public ParserTest<AddressParser>
{
protected:
	template<typename... Exprs>
	void expression_is(const ExpressionToken& expr0, const Exprs&... exprs)
	{
		ASSERT_TRUE(parser_.expression());
		ASSERT_FALSE(parser_.w_value());

		ASSERT_EQ(ExpressionBuilder::Build(expr0, exprs...), *parser_.expression());
	}

	template<typename... WBuilders>
	void literal_is(const WTokenBuilder& w_builder, const WBuilders&... builders)
	{
		ASSERT_TRUE(parser_.w_value());
		ASSERT_FALSE(parser_.expression());

		ASSERT_EQ(WTokenBuilder::Build(w_builder, builders...), *parser_.w_value());
	}
};

} // namespace

TEST_F(AddressParserTest, Empty_String_Is_Valid_Address)
{
	parse("");
	ASSERT_TRUE(parser_.empty());
}

TEST_F(AddressParserTest, White_Spaces_Are_Ignored)
{
	parse(" \t\t");
	ASSERT_TRUE(parser_.empty());
}

TEST_F(AddressParserTest, Parses_Expression)
{
	parse(" LABEL * 2 xxxxxx");

	expression_is(BinaryToken("LABEL", "*"), Token("2"));

	reminder_stream_is(" xxxxxx");
}

TEST_F(AddressParserTest, Parses_Literal)
{
	parse(" =8= * 2");

	literal_is(WToken()
		.expression_is(Token("8")));

	reminder_stream_is(" * 2");
}
