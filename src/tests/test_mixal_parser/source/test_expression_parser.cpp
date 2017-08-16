#include <mixal/expression_parser.h>

#include "parser_test_fixture.h"
#include "expression_builders.h"

using namespace mixal;

namespace {

std::string MakeLongestSymbol()
{
	return std::string(k_max_symbol_length, 'A');
}

class ExpressionParserTest :
	public ParserTest<ExpressionParser>
{
protected:
	template<typename... Exprs>
	void tokens_are(const ExpressionToken& expr0, const Exprs&... exprs)
	{
		ASSERT_EQ(ExpressionBuilder::Build(expr0, exprs...), parser_.expression());
	}
};

} // namespace

TEST_F(ExpressionParserTest, Expression_Can_Contain_Only_Number)
{
	parse(" 42  ");
	tokens_are(Token("42"));
	reminder_stream_is("  ");
}

TEST_F(ExpressionParserTest, Expression_Can_Contain_Only_Symbol)
{
	parse("LABEL");
	tokens_are(Token("LABEL"));
	reminder_stream_is("");
}

TEST_F(ExpressionParserTest, Expression_Can_Contain_Only_Current_Address_Counter)
{
	parse(" *");
	tokens_are(Token("*"));
	reminder_stream_is("");
}

TEST_F(ExpressionParserTest, Expression_Can_Contain_Basic_Expression_With_Unary_Op_In_The_Beginning)
{
	parse(" -*");
	tokens_are(UnaryToken("-", "*"));
	reminder_stream_is("");
}

TEST_F(ExpressionParserTest, Expression_With_Only_Unary_Op_Will_Fail)
{
	parse_error("+");
}

TEST_F(ExpressionParserTest, Differentiate_Current_Address_Symbol_From_Multiply_Binary_Operation)
{
	parse("***");
	tokens_are(BinaryToken("*", "*"), Token("*"));
	reminder_stream_is("");
}

TEST_F(ExpressionParserTest, Full_Token_Is_Unary_Op_With_Basic_Expression_And_Binary_Op)
{
	parse("+ * - 3");
	tokens_are(Token("+", "*", "-"), Token("3"));
	reminder_stream_is("");
}

TEST_F(ExpressionParserTest, Parses_All_Token_To_The_Vector_With_Left_To_Right_Order)
{
	//         1 |2|   3|4|
	parse("  -1 + 5*20 / 6  ");

	tokens_are(
		Token("-", "1", "+"),
		BinaryToken("5", "*"),
		BinaryToken("20", "/"),
		Token("6"));

	reminder_stream_is("  ");
}

TEST_F(ExpressionParserTest, Parses_Mix_Field_Specification_As_Binary_Op)
{
	parse("1:3");

	tokens_are(
		BinaryToken("1", ":"),
		Token("3"));

	reminder_stream_is("");
}

TEST_F(ExpressionParserTest, Parses_Special_MIXAL_Binary_Operations)
{
	parse("1 // 3");

	tokens_are(
		BinaryToken("1", "//"),
		Token("3"));
	
	reminder_stream_is("");
}

TEST_F(ExpressionParserTest, Parses_As_Much_As_Possible)
{
	auto symbol = MakeLongestSymbol();
	auto too_long_symbol = symbol + symbol;
	
	parse(too_long_symbol);

	tokens_are(Token(symbol));

	reminder_stream_is(symbol);
}

TEST_F(ExpressionParserTest, Fails_To_Parse_Unknow_Expression)
{
	parse_error("(1:5)");
}

TEST_F(ExpressionParserTest, Parses_Valid_Expression_Part_And_Leaves_Rest_Unchanged)
{
	parse("00001 =LABEL=");
	tokens_are(Token("00001"));
	reminder_stream_is(" =LABEL=");
}

TEST_F(ExpressionParserTest, Partial_Expressions_Are_Not_Valid)
{
	// `*` is multiplication in this case,
	// so expression on the rigth of `*` should be valid
	// basic expression, but it's not (`(1:1)` is field)
	// 
	// #TODO: errors reporting
	parse_error("XXXX * (1:1)");
}

TEST_F(ExpressionParserTest, Local_Symbol_Is_Parsed_As_Symbol_Instead_Of_Number)
{
	parse("9F");
	tokens_are(Token("9F"));
}
