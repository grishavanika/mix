#include <mixal/line_parser.h>

#include "parser_test_fixture.h"

using namespace mixal;

namespace {

class LineParserTest :
	public ParserTest<LineParser>
{
};

} // namespace

TEST_F(LineParserTest, Parses_Line_With_Comment_Leaving_Comment_Unchanged)
{
	parse("* comment   ");
	ASSERT_TRUE(parser_.comment());
	ASSERT_EQ("* comment   ", *parser_.comment());
}

TEST_F(LineParserTest, Ignores_Line_Left_White_Spaces_For_Comment_Line)
{
	parse("       * comment   ");
	ASSERT_TRUE(parser_.comment());
	ASSERT_EQ("* comment   ", *parser_.comment());
}

TEST_F(LineParserTest, Parses_Line_With_Single_Operation)
{
	parse("STA");
	ASSERT_TRUE(parser_.operation());
	ASSERT_EQ(OperationId::STA, parser_.operation()->id());
	ASSERT_TRUE(parser_.address());
	ASSERT_FALSE(parser_.comment());
}

TEST_F(LineParserTest, Line_With_Single_Operation_Can_Contain_WhiteSpaces)
{
	{
		parse("  LDA");
		ASSERT_TRUE(parser_.operation());
		ASSERT_EQ(OperationId::LDA, parser_.operation()->id());
		ASSERT_TRUE(parser_.address());
		ASSERT_FALSE(parser_.comment());
	}

	{
		parse("  LD2N     ");
		ASSERT_TRUE(parser_.operation());
		ASSERT_EQ(OperationId::LD2N, parser_.operation()->id());
		ASSERT_TRUE(parser_.address());
		ASSERT_FALSE(parser_.comment());
	}
}

TEST_F(LineParserTest, Line_Can_Not_Contain_Only_Label)
{
	parse_error("LABEL  ");
}

TEST_F(LineParserTest, Address_Column_Can_Be_Empty)
{
	parse("LABEL  JNE");

	ASSERT_TRUE(parser_.label());
	ASSERT_TRUE(parser_.operation());
	ASSERT_EQ(OperationId::JNE, parser_.operation()->id());
	ASSERT_TRUE(parser_.address());
	ASSERT_FALSE(parser_.comment());

	parse("8H  INC1   ");
	ASSERT_TRUE(parser_.label());
	ASSERT_TRUE(parser_.operation());
	ASSERT_EQ(OperationId::INC1, parser_.operation()->id());
	ASSERT_TRUE(parser_.address());
	ASSERT_FALSE(parser_.comment());
}

TEST_F(LineParserTest, Can_Contain_In_Line_Comment_That_Starts_With_Invalid_MIXAL_Code)
{
	parse("ADD some comment  ");

	ASSERT_FALSE(parser_.label());
	ASSERT_TRUE(parser_.operation());
	ASSERT_EQ(OperationId::ADD, parser_.operation()->id());
	ASSERT_TRUE(parser_.address());
	ASSERT_TRUE(parser_.comment());
	ASSERT_EQ("some comment  ", *parser_.comment());
}

TEST_F(LineParserTest, Splits_Label_Op_Address_And_Comment_Into_Separate_Parts_With_No_WhiteSpaces)
{
	parse("START   IN      BUF (TERM)   read a block (70 chars)");

	ASSERT_TRUE(parser_.label());
	ASSERT_EQ("START", parser_.label()->name());

	ASSERT_TRUE(parser_.operation());
	ASSERT_EQ(OperationId::IN, parser_.operation()->id());

	ASSERT_TRUE(parser_.address());
	ASSERT_EQ("BUF (TERM)", parser_.address()->str());
	
	ASSERT_TRUE(parser_.comment());
	ASSERT_EQ("read a block (70 chars)", *parser_.comment());
}

TEST_F(LineParserTest, When_Label_Name_Is_Valid_Operation_And_Line_Has_Valid_Operation_With_Non_Empty_ADDRESS_Part_Then_Parses_Them_As_Label_And_Operation)
{
	parse("IN OUT 1");
	ASSERT_TRUE(parser_.label());
	ASSERT_EQ("IN", parser_.label()->name());
	
	ASSERT_TRUE(parser_.operation());
	ASSERT_EQ(OperationId::OUT, parser_.operation()->id());
}

TEST_F(LineParserTest, When_Label_Name_Is_Valid_Operation_And_Line_Has_Valid_Operation_With_EMPTY_ADDRESS_Part_Then_Parses_As_Operation_And_Address)
{
	parse("OUT IN");
	ASSERT_FALSE(parser_.label());

	ASSERT_TRUE(parser_.operation());
	ASSERT_EQ(OperationId::OUT, parser_.operation()->id());

	ASSERT_TRUE(parser_.address());
	ASSERT_EQ("IN", parser_.address()->str());
}

