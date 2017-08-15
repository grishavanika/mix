#include <mixal/label_parser.h>
#include <mixal/parse_exceptions.h>
#include <mixal/parsers_utils.h>

#include <gtest/gtest.h>

using namespace mixal;

TEST(LabelParser, Empty_String_IsValid_Label)
{
	LabelParser p;
	p.parse("");
	ASSERT_FALSE(p.is_local_symbol());
	ASSERT_EQ("", p.name());
	ASSERT_TRUE(p.empty());
}

TEST(LabelParser, Should_Contain_At_Least_One_Alphabetic_Char)
{
	LabelParser p;

	p.parse("0101A324");
	ASSERT_EQ("0101A324", p.name());
	ASSERT_FALSE(p.is_local_symbol());

	ASSERT_THROW(
	{
		p.parse("03421");
	}, ParseError);
}

TEST(LabelParser, White_Spaces_Are_Stripped)
{
	LabelParser p;

	p.parse("   CC01XX  ");
	ASSERT_EQ("CC01XX", p.name());

	p.parse("  2H ");
	ASSERT_EQ("2H", p.name());
}

TEST(LabelParser, Too_Long_Labels_Throws_InvalidLabel)
{
	std::string str(k_max_symbol_length + 1, 'X');
	ASSERT_GT(str.size(), k_max_symbol_length);

	LabelParser p;
	ASSERT_THROW(
	{
		p.parse(str);
	}, ParseError);
}

TEST(LabelParser, Local_Symbol_ID_Is_Valid_For_Valid_Local_Symbol)
{
	LabelParser p;
	p.parse("3H");
	ASSERT_TRUE(p.is_local_symbol());
	ASSERT_TRUE(p.local_symbol_id());
	ASSERT_EQ(3, *p.local_symbol_id());
}

TEST(LabelParser, Local_Symbol_With_Number_Instead_Of_One_Digit_Is_Usual_Label)
{
	LabelParser p;
	p.parse("23H");
	ASSERT_FALSE(p.is_local_symbol());
	ASSERT_EQ("23H", p.name());
}

TEST(LabelParser, Backward_Local_Symbol_Is_Usual_Label)
{
	LabelParser p;
	p.parse("0B");
	ASSERT_FALSE(p.is_local_symbol());
	ASSERT_EQ("0B", p.name());
}

TEST(LabelParser, Forward_Local_Symbol_Is_Usual_Label)
{
	LabelParser p;
	p.parse("9F");
	ASSERT_FALSE(p.is_local_symbol());
	ASSERT_EQ("9F", p.name());
}

TEST(LabelParser, Only_MIX_Chars_Are_Accepted)
{
	LabelParser p;

	ASSERT_THROW(
	{
		p.parse("xx");
	}, ParseError);
}
