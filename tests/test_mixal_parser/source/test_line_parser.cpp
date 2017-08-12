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


