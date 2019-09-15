#include <mixal_parse/line_parser.h>

#include "parser_test_fixture.h"

using namespace mixal_parse;

namespace {

class LineParserTest :
	public ParserTest<LineParser>
{

protected:
	void has_comment(std::string_view comment)
	{
		ASSERT_TRUE(parser_.comment());
		ASSERT_EQ(comment, *parser_.comment());
	}

	void has_no_comment()
	{
		ASSERT_FALSE(parser_.comment());
	}

	void has_operation(OperationId id)
	{
		ASSERT_TRUE(parser_.operation_parser());
		ASSERT_EQ(id, parser_.operation_parser()->operation().id());
	}

	void has_address_str(std::string_view str)
	{
		ASSERT_TRUE(parser_.address());
		ASSERT_EQ(str, parser_.address()->str());
	}

	void has_no_label()
	{
		ASSERT_FALSE(parser_.label_parser());
	}

	void has_label(std::string_view name)
	{
		ASSERT_TRUE(parser_.label_parser());
		ASSERT_EQ(name, parser_.label_parser()->label().name());
	}
};

} // namespace

TEST_F(LineParserTest, Parses_Line_With_Comment_Leaving_Comment_Unchanged)
{
	parse("* comment   ");
	has_comment("* comment   ");
}

TEST_F(LineParserTest, Ignores_Line_Left_White_Spaces_For_Comment_Line)
{
	parse("       * comment   ");
	has_comment("* comment   ");
}

TEST_F(LineParserTest, Parses_Line_With_Single_Operation)
{
	parse("STA");
	has_operation(OperationId::STA);
	has_address_str("");
	has_no_comment();
}

TEST_F(LineParserTest, Line_With_Single_Operation_Can_Contain_WhiteSpaces)
{
	{
		parse("  LDA");
		has_operation(OperationId::LDA);
		has_address_str("");
		has_no_comment();
	}

	{
		parse("  LD2N     ");
		has_operation(OperationId::LD2N);
		has_address_str("");
		has_no_comment();
	}
}

TEST_F(LineParserTest, Line_Can_Not_Contain_Only_Label)
{
	parse_error("LABEL  ");
}

TEST_F(LineParserTest, Address_Column_Can_Be_Empty)
{
	parse("LABEL  JNE");
	has_address_str("");
}

TEST_F(LineParserTest, Can_Contain_In_Line_Comment_That_Starts_With_Invalid_MIXAL_Code)
{
	parse("ADD some comment  ");

	has_no_label();
	has_operation(OperationId::ADD);
	has_address_str("");
	has_comment("some comment  ");
}

TEST_F(LineParserTest, Splits_Label_Op_Address_And_Comment_Into_Separate_Parts_With_No_WhiteSpaces)
{
	parse("START   IN      BUF (TERM)   read a block (70 chars)");

	has_label("START");
	has_operation(OperationId::IN_);
	has_address_str("BUF (TERM)");
	has_comment("read a block (70 chars)");
}

TEST_F(LineParserTest, When_Label_Name_Is_Valid_Operation_And_Line_Has_Valid_Operation_With_Non_Empty_ADDRESS_Part_Then_Parses_Them_As_Label_And_Operation)
{
	parse("IN OUT 1");

	has_label("IN");
	has_operation(OperationId::OUT_);
	has_address_str("1");
}

TEST_F(LineParserTest, When_Label_Name_Is_Valid_Operation_And_Line_Has_Valid_Operation_With_EMPTY_ADDRESS_Part_Then_Parses_As_Operation_And_Address)
{
	parse("OUT IN");

	has_no_label();
	has_operation(OperationId::OUT_);
	has_address_str("IN");
}

