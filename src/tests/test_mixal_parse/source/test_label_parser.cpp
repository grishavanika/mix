#include <mixal_parse/label_parser.h>

#include "parser_test_fixture.h"

using namespace mixal_parse;

namespace {

class LabelParserTest :
	public ParserTest<LabelParser>
{
};

} // namespace

TEST_F(LabelParserTest, Empty_String_IsValid_Label)
{
	parse("");
	ASSERT_FALSE(parser_.label().is_local_symbol());
	ASSERT_EQ("", parser_.label().name());
	ASSERT_TRUE(parser_.label().empty());
}

TEST_F(LabelParserTest, Should_Contain_At_Least_One_Alphabetic_Char)
{
	parse("0101A324");
	ASSERT_EQ("0101A324", parser_.label().name());
	ASSERT_FALSE(parser_.label().is_local_symbol());

	parse_error("03421");
}

TEST_F(LabelParserTest, White_Spaces_Are_Stripped)
{
	parse("   CC01XX  ");
	ASSERT_EQ("CC01XX", parser_.label().name());

	parse("  2H ");
	ASSERT_EQ("2H", parser_.label().name());
}

TEST_F(LabelParserTest, Fails_To_Parse_Too_Long_Labels)
{
	std::string str(k_max_symbol_length + 1, 'X');
	ASSERT_GT(str.size(), k_max_symbol_length);

	parse_error(str);
}

TEST_F(LabelParserTest, Fetches_Local_Symbol_ID_For_Valid_Local_Symbol)
{
	parse("3H");
	ASSERT_TRUE(parser_.label().is_local_symbol());
	ASSERT_EQ(3, parser_.label().local_id());
}

TEST_F(LabelParserTest, Local_Symbol_With_Unsupported_Number_Is_Error)
{
	ASSERT_GT(1024, k_max_local_symbol_id);
	parse_error("1024H");
}

TEST_F(LabelParserTest, Backward_Local_Symbol_Is_Error)
{
	parse_error("0B");
}

TEST_F(LabelParserTest, Forward_Local_Symbol_Is_Error)
{
	parse_error("9F");
}

TEST_F(LabelParserTest, Only_Valid_MIX_Chars_Are_Accepted)
{
	parse_error("xx");
}
