#include <mixal/index_parser.h>

#include "parser_test_fixture.h"

using namespace mixal;

namespace {

class IndexParserTest :
	public ParserTest<IndexParser>
{
};

} // namespace

TEST_F(IndexParserTest, Empty_String_Is_Valid_Index)
{
	parse("");
	ASSERT_TRUE(parser_.empty());
}

TEST_F(IndexParserTest, Empty_String_After_Comma_Is_Not_Valid_Index)
{
	parse_error(" , ");
}

TEST_F(IndexParserTest, Parses_Expression_After_Comma)
{
	parse(" , *** ");
	reminder_stream_is(" ");

	ASSERT_TRUE(parser_.expression());
	ASSERT_EQ(2u, parser_.expression()->tokens.size());

	ASSERT_FALSE(parser_.empty());
}

