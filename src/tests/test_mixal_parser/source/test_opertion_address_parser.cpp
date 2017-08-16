#include <mixal/operation_address_parser.h>

#include <memory>

#include <gtest/gtest.h>

using namespace mixal;

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
protected:
	std::unique_ptr<OperationAddressParser> parser_;
	std::string_view rest_of_parsed_stream_;
};

} // namespace

///////////////////////////////////////////////////////////////////////////////
// TODO:
// - add expectation builder 
//	Builder{}.mix()
//			.address_is(...)
//			.index_is(...)
//			.field_is()
//
// - do not cut spaces for Address/Index/Field parsers

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Address_And_Index_And_Field_Specified)
{
	parse(OperationId::STX, " 0,4 (1:4) comment");
	reminder_stream_is(" comment");
	ASSERT_TRUE(parser_->is_mix_operation());
}

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Address_And_Index_Specified)
{
	parse(OperationId::STX, " XXXXX,4      comment");
	// Note: spaces where eaten by Field Parser
	reminder_stream_is("comment");
	ASSERT_TRUE(parser_->is_mix_operation());
}

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Only_Address_Specified)
{
	parse(OperationId::STX, " =1000=         comment");
	reminder_stream_is("comment");
	ASSERT_TRUE(parser_->is_mix_operation());
}

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Index_And_Field_Specified)
{
	parse(OperationId::STX, " ,4 (1:4) comment");
	reminder_stream_is(" comment");
	ASSERT_TRUE(parser_->is_mix_operation());
}

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Only_Field_Specified)
{
	parse(OperationId::STX, " (1:4)   comment");
	reminder_stream_is("   comment");
	ASSERT_TRUE(parser_->is_mix_operation());
}

TEST_F(OperationAddressParserTest, Parses_MIX_Operation_With_Only_Index_Specified)
{
	parse(OperationId::STX, " ,4     comment");
	reminder_stream_is("comment");
	ASSERT_TRUE(parser_->is_mix_operation());
}

TEST_F(OperationAddressParserTest, Parses_MIXAL_Operation)
{
	parse(OperationId::EQU, " LABEL(1:0),1000      comment");
	// Note: spaces where eaten by Field Parser
	reminder_stream_is("comment");
	ASSERT_TRUE(parser_->is_mixal_operation());
}

TEST_F(OperationAddressParserTest, Parses_MIXAL_ALF_Operation_With_Two_Spaces)
{
	parse(OperationId::ALF, "  AAAAA   comment");
	reminder_stream_is("   comment");
	ASSERT_TRUE(parser_->is_mixal_operation());
	ASSERT_TRUE(parser_->mixal());
	ASSERT_TRUE(parser_->mixal()->alf_text);
	ASSERT_EQ("AAAAA", *parser_->mixal()->alf_text);
}

TEST_F(OperationAddressParserTest, Parses_MIXAL_ALF_Operation_With_One_Spaces_And_Next_Non_Space_Char)
{
	parse(OperationId::ALF, " AAAAA   comment");
	reminder_stream_is("   comment");
	ASSERT_TRUE(parser_->is_mixal_operation());
	ASSERT_TRUE(parser_->mixal());
	ASSERT_TRUE(parser_->mixal()->alf_text);
	ASSERT_EQ("AAAAA", *parser_->mixal()->alf_text);
}


TEST_F(OperationAddressParserTest, Fails_To_Parse_Too_Short_ALF_String)
{
	parse_error(OperationId::ALF, "  AA");
}
