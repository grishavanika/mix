#include <mixal/address_parser.h>

#include "parser_test_fixture.h"
#include "expression_builders.h"

using namespace mixal;

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
	void w_value_is(const WTokenBuilder& w_builder, const WBuilders&... builders)
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

	parse(" \t\t");
	ASSERT_TRUE(parser_.empty());
}

TEST_F(AddressParserTest, Parses_Expression_If_Its_Expression)
{
	parse(" LABEL * 2 xxxxxx");

	expression_is(BinaryToken("LABEL", "*"), Token("2"));

	reminder_stream_is(" xxxxxx");
}

TEST_F(AddressParserTest, Parses_Literal_If_Its_Literal)
{
	parse(" =8= * 2");

	w_value_is(WToken()
		.expression_is(Token("8")));

	reminder_stream_is(" * 2");
}
