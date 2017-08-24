#include <mixal/line_translator.h>
#include <mixal/translator.h>
#include <mixal/exceptions.h>

#include <mixal_parse/line_parser.h>

#include <mix/command.h>

#include <gtest/gtest.h>

using namespace mixal;
using namespace mix;
using namespace mixal_parse;

namespace {

class LineTranslatorTest :
	public ::testing::Test
{
protected:
	FutureTranslatedWordRef translate(const char* line)
	{
		LineParser parser;
		const auto pos = parser.parse_stream(line);
		if (IsInvalidStreamPosition(pos))
		{
			throw std::logic_error{"bad test input line"};
		}
		
		return TranslateLine(translator_, parser).word_ref;
	}

protected:
	Translator translator_;
};

} // namespace

TEST_F(LineTranslatorTest, EQU_With_Label_Defines_Symbol_With_Label_Name)
{
	translator_.set_current_address(400);
	auto word = translate("X EQU 1000");

	ASSERT_EQ(nullptr, word);
	ASSERT_EQ(1000, translator_.query_defined_symbol("X"));
	ASSERT_EQ(400, translator_.current_address());
}

TEST_F(LineTranslatorTest, EQU_Without_Label_Is_NOP)
{
	translator_.set_current_address(400);
	auto word = translate("EQU 1000");

	ASSERT_EQ(nullptr, word);
	ASSERT_EQ(400, translator_.current_address());
}

TEST_F(LineTranslatorTest, ORIG_Changes_Current_Address_Value_To_Specified_In_Address_Columnt_Value)
{
	translator_.set_current_address(400);
	auto word = translate("ORIG 3000");

	ASSERT_EQ(nullptr, word);
	ASSERT_EQ(3000, translator_.current_address());
}

TEST_F(LineTranslatorTest, ORIG_Defines_Label_Symbol_Before_Changing_The_Current_Address)
{
	translator_.set_current_address(400);
	auto word = translate("TABLE ORIG *+100");

	ASSERT_EQ(nullptr, word);
	ASSERT_EQ(400, translator_.query_defined_symbol("TABLE"));
	ASSERT_EQ(500, translator_.current_address());
}

TEST_F(LineTranslatorTest, DISABLED_ORIG_With_Local_Symbol_Label_Is_Unspecified)
{
	// #TODO: what is the behavior ?
	auto word = translate("2H ORIG 100");

	ASSERT_EQ(nullptr, word);
}

TEST_F(LineTranslatorTest, TODO_Name_This_Case)
{
	auto word1 = translate("MAXIMUM STJ EXIT");

	ASSERT_NE(nullptr, word1);
	ASSERT_FALSE(word1->is_ready());
	ASSERT_EQ(std::vector<Symbol>{"EXIT"}, word1->forward_references);

	translator_.set_current_address(3009);
	auto word2 = translate("EXIT JMP *");

	ASSERT_NE(nullptr, word2);
	ASSERT_TRUE(word2->is_ready());

	ASSERT_TRUE(word1->is_ready());
	ASSERT_EQ(3009, Command{word1->value}.address());
}

