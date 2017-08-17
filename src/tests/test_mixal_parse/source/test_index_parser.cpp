#include <mixal_parse/index_parser.h>

#include "parser_test_fixture.h"
#include "expression_builders.h"

using namespace mixal_parse;

namespace {

class IndexParserTest :
	public ParserTest<IndexParser>
{
protected:
	template<typename... Exprs>
	void expression_is(const Exprs&... exprs)
	{
		ASSERT_TRUE(parser_.expression());
		ASSERT_EQ(ExpressionBuilder::Build(exprs...), *parser_.expression());
	}
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

	expression_is(BinaryToken("*", "*"), Token("*"));

	reminder_stream_is(" ");
}

