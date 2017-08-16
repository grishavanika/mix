#include <mixal/address_parser.h>

#include "parser_test_fixture.h"

using namespace mixal;

namespace {

class AddressParserTest :
	public ParserTest<AddressParser>
{
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
	reminder_stream_is(" xxxxxx");

	ASSERT_TRUE(parser_.expression());
	ASSERT_EQ(2u, parser_.expression()->tokens.size());

	ASSERT_FALSE(parser_.empty());
	ASSERT_FALSE(parser_.w_value());
}

TEST_F(AddressParserTest, Parses_Literal_If_Its_Literal)
{
	parse(" =8= * 2");
	reminder_stream_is(" * 2");

	ASSERT_TRUE(parser_.w_value());
	ASSERT_EQ(1u, parser_.w_value()->tokens.size());

	ASSERT_FALSE(parser_.empty());
	ASSERT_FALSE(parser_.expression());
}
