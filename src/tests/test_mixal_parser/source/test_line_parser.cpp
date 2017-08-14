#include <mixal/line_parser.h>
#include <mixal/parse_exceptions.h>

#include <gtest/gtest.h>

using namespace mixal;

TEST(LineParser, Parses_Line_With_Comment_Leaving_Comment_Unchanged)
{
	LineParser p;
	p.parse("* comment   ");
	ASSERT_TRUE(p.comment());
	ASSERT_EQ("* comment   ", *p.comment());
}

TEST(LineParser, Ignores_Line_Left_White_Spaces_For_Comment_Line)
{
	LineParser p;
	p.parse("       * comment");
	ASSERT_TRUE(p.comment());
	ASSERT_EQ("* comment", *p.comment());
}

TEST(LineParser, Parses_Line_With_Single_Operation)
{
	LineParser p;
	p.parse("STA");
	ASSERT_TRUE(p.operation());
	ASSERT_EQ(OperationId::STA, p.operation()->id());
	ASSERT_TRUE(p.address_str().empty());
	ASSERT_FALSE(p.comment());
}

TEST(LineParser, Line_With_Single_Operation_Can_Contain_WhiteSpaces)
{
	{
		LineParser p;
		p.parse("  LDA");
		ASSERT_TRUE(p.operation());
		ASSERT_EQ(OperationId::LDA, p.operation()->id());
		ASSERT_TRUE(p.address_str().empty());
		ASSERT_FALSE(p.comment());
	}

	{
		LineParser p;
		p.parse("  LD2N     ");
		ASSERT_TRUE(p.operation());
		ASSERT_EQ(OperationId::LD2N, p.operation()->id());
		ASSERT_TRUE(p.address_str().empty());
		ASSERT_FALSE(p.comment());
	}
}

TEST(LineParser, Line_Can_Not_Contain_Only_Label)
{
	LineParser p;

	ASSERT_THROW({
		p.parse("LABEL  ");
	}, InvalidLine);
}

TEST(LineParser, Address_Column_Can_Be_Empty)
{
	LineParser p;
	p.parse("LABEL  JNE");

	ASSERT_TRUE(p.label());
	ASSERT_TRUE(p.operation());
	ASSERT_EQ(OperationId::JNE, p.operation()->id());
	ASSERT_TRUE(p.address_str().empty());
	ASSERT_FALSE(p.comment());

	p.parse("8H  INC1   ");
	ASSERT_TRUE(p.label());
	ASSERT_TRUE(p.operation());
	ASSERT_EQ(OperationId::INC1, p.operation()->id());
	ASSERT_TRUE(p.address_str().empty());
	ASSERT_FALSE(p.comment());
}

TEST(LineParser, Can_Contain_In_Line_Comments_That_Starts_With_Lower_Case)
{
	LineParser p;
	p.parse("ADD some comment  ");

	ASSERT_FALSE(p.label());
	ASSERT_TRUE(p.operation());
	ASSERT_EQ(OperationId::ADD, p.operation()->id());
	ASSERT_TRUE(p.address_str().empty());
	ASSERT_TRUE(p.comment());
	ASSERT_EQ("some comment  ", *p.comment());
}

TEST(LineParser, Splits_Label_Op_Address_And_Comment_Into_Separate_Parts_With_No_WhiteSpaces)
{
	LineParser p;

	p.parse("START   IN      BUF (TERM)   read a block (70 chars)");

	ASSERT_TRUE(p.label());
	ASSERT_EQ("START", p.label()->name());

	ASSERT_TRUE(p.operation());
	ASSERT_EQ(OperationId::IN, p.operation()->id());

	ASSERT_EQ("BUF (TERM)", p.address_str());
	
	ASSERT_TRUE(p.comment());
	ASSERT_EQ("read a block (70 chars)", *p.comment());
}
