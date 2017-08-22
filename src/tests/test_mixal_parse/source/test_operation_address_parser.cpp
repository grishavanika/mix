#include <mixal_parse/operation_address_parser.h>

#include <memory>

#include <gtest/gtest.h>

#include "expression_builders.h"

using namespace mixal_parse;

namespace {

class OperationAddressParserTest :
	public ::testing::Test
{
protected:
	void parse(OperationId id, std::string_view str)
	{
		parser_ = std::make_unique<OperationAddressParser>(id);
		const auto pos = parser_->parse_stream(str);
		ASSERT_FALSE(IsInvalidStreamPosition(pos));
		rest_of_parsed_stream_ = str.substr(pos);
	}

	void parse_error(OperationId id, std::string_view str)
	{
		parser_ = std::make_unique<OperationAddressParser>(id);
		const auto pos = parser_->parse_stream(str);
		ASSERT_TRUE(IsInvalidStreamPosition(pos));
	}

	void reminder_stream_is(std::string_view rest_of_stream)
	{
		ASSERT_NE(nullptr, parser_);
		ASSERT_EQ(rest_of_stream, rest_of_parsed_stream_);
	}

	void result_is(MIXALOpBuilder& mixal_builder)
	{
		ASSERT_TRUE(parser_->is_mixal_operation());
		ASSERT_TRUE(parser_->mixal());

		const auto& mixal = *parser_->mixal();

		ASSERT_EQ(mixal_builder.alf_text, mixal.alf_text);

		if (!mixal_builder.w_value)
		{
			ASSERT_FALSE(mixal.w_value_parser);
			return;
		}

		ASSERT_EQ(*mixal_builder.w_value, mixal.w_value_parser->value());
	}

	void result_is(MIXOpBuilder& mix_builder)
	{
		ASSERT_TRUE(parser_->is_mix_operation());
		ASSERT_TRUE(parser_->mix());

		const auto& mix = *parser_->mix();
	
		ASSERT_EQ(mix_builder.index_expr.value_or(Expression{}),
			mix.index_parser.index().expression());
		ASSERT_EQ(mix_builder.field_expr.value_or(Expression{}),
			mix.field_parser.field().expression());

		ASSERT_EQ(mix_builder.address_expr.value_or(Expression{}),
			mix.address_parser.address().expression());
		ASSERT_EQ(mix_builder.address_w_value.value_or(WValue{}),
			mix.address_parser.address().w_value());
	}

protected:
	std::unique_ptr<OperationAddressParser> parser_;
	std::string_view rest_of_parsed_stream_;
};

} // namespace

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Non_Empy_Address_Index_Field)
{
	parse(OperationId::STX, " 0,4 (1:4) comment");
	
	result_is(MIX()
		.address_is(Token("0"))
		.index_is(Token("4"))
		.field_is(
			BinaryToken("1", ":"),
			Token("4")));
	
	reminder_stream_is(" comment");
}

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Address_And_Index)
{
	parse(OperationId::STX, " XXXXX,4      comment");

	result_is(MIX()
		.address_is(Token("XXXXX"))
		.index_is(Token("4")));

	// Note: spaces where eaten by Field Parser
	reminder_stream_is("comment");
}

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Only_Address)
{
	parse(OperationId::STX, " =1000,100=         comment");
	
	result_is(MIX()
		.address_is(
			WToken()
				.expression_is(
					Token("1000")),
			WToken()
				.expression_is(
					Token("100"))));

	reminder_stream_is("comment");
}

TEST_F(OperationAddressParserTest, Parses_Complex_Address_Index_Field_Expressions)
{
	parse(OperationId::STX, " =1000(1:5)=,*** (+1*K)     comment");

	result_is(MIX()
		.address_is(WToken()
			.expression_is(
				Token("1000"))
			.field_is(
				BinaryToken("1", ":"),
				Token("5")))
		.index_is(
			BinaryToken("*", "*"),
			Token("*"))
		.field_is(
			Token("+", "1", "*"),
			Token("K")));
	
	reminder_stream_is("     comment");
}

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Index_Field)
{
	parse(OperationId::STX, " ,4 (1:4) comment");
	
	result_is(MIX()
		.index_is(Token("4"))
		.field_is(
			BinaryToken("1", ":"),
			Token("4")));
	
	reminder_stream_is(" comment");
}

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Only_Field)
{
	parse(OperationId::STX, " (1:4)   comment");

	result_is(MIX()
		.field_is(
			BinaryToken("1", ":"),
			Token("4")));

	reminder_stream_is("   comment");
}

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Only_Index)
{
	parse(OperationId::STX, " ,4     comment");

	result_is(MIX()
		.index_is(Token("4")));

	reminder_stream_is("comment");
}

TEST_F(OperationAddressParserTest, Parses_MIXAL_Operation)
{
	parse(OperationId::EQU, " LABEL(1:0),1000      comment");
	
	result_is(MIXAL()
		.token(WToken()
			.expression_is(
				Token("LABEL"))
			.field_is(
				BinaryToken("1", ":"), Token("0")))
		.token(WToken()
			.expression_is(
				Token("1000"))));

	// Note: spaces where eaten by Field Parser
	reminder_stream_is("comment");
}

TEST_F(OperationAddressParserTest, Parses_MIXAL_ALF_Operation_With_Two_Spaces)
{
	parse(OperationId::ALF, "  AAAAA   comment");
	
	result_is(MIXAL()
		.ALF_text_is("AAAAA"));

	reminder_stream_is("   comment");
}

TEST_F(OperationAddressParserTest, Parses_MIXAL_ALF_Operation_With_One_Spaces_And_Next_Non_Space_Char)
{
	parse(OperationId::ALF, " AAAAA   comment");

	result_is(MIXAL()
		.ALF_text_is("AAAAA"));

	reminder_stream_is("   comment");
}

TEST_F(OperationAddressParserTest, ALF_String_Should_Be_5_Chars_Long)
{
	parse_error(OperationId::ALF, "  AA");
}

TEST_F(OperationAddressParserTest, ALF_Space_Is_Any_White_Space)
{
	parse(OperationId::ALF, "\tFIRST");
	
	result_is(MIXAL()
		.ALF_text_is("FIRST"));
}

TEST_F(OperationAddressParserTest, ALF_Can_Be_Enclosed_With_Spaces)
{
	parse(OperationId::ALF, R"("KKKKK"comment goes here)");
	reminder_stream_is("comment goes here");

	result_is(MIXAL()
		.ALF_text_is("KKKKK"));
}

TEST_F(OperationAddressParserTest, ALF_Fails_To_Parse_Half_Opened_Quotes)
{
	parse_error(OperationId::ALF, "(\"KKKKK");
}

TEST_F(OperationAddressParserTest, ALF_Parses_String_With_Quotes_At_The_End_Of_String)
{
	parse(OperationId::ALF, R"("AAAAA")");
	reminder_stream_is("");

	result_is(MIXAL()
		.ALF_text_is("AAAAA"));
}
