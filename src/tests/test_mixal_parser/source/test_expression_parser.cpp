#include <mixal/expression_parser.h>
#include <mixal/parse_exceptions.h>

#include <gtest/gtest.h>

using namespace mixal;

namespace {

ExpressionToken BuildToken(
	std::optional<UnaryOperation> unary_op,
	BasicExpression basic_expr,
	std::optional<BinaryOperation> binary_op)
{
	return {unary_op, basic_expr, binary_op};
}

ExpressionToken BuildToken(
	BasicExpression basic_expr)
{
	return BuildToken({}, basic_expr, {});
}

std::string MakeTooLongSymbol()
{
	return std::string(2 * k_max_symbol_length, 'A');
}

} // namespace

TEST(ExpressionParser, Expression_Can_Contain_Only_Number)
{
	ExpressionParser p;
	p.parse(" 42  ");
	const auto& tokens = p.expression().tokens;
	ASSERT_EQ(1u, tokens.size());
	ASSERT_EQ(BuildToken("42"), tokens.front());
}

TEST(ExpressionParser, Expression_Can_Contain_Only_Symbol)
{
	ExpressionParser p;
	p.parse("LABEL");
	const auto& tokens = p.expression().tokens;
	ASSERT_EQ(1u, tokens.size());
	ASSERT_EQ(BuildToken("LABEL"), tokens.front());
}

TEST(ExpressionParser, Expression_Can_Contain_Only_Current_Address_Counter)
{
	ExpressionParser p;
	p.parse(" *");
	const auto& tokens = p.expression().tokens;
	ASSERT_EQ(1u, tokens.size());
	ASSERT_EQ(BuildToken("*"), tokens.front());
}

TEST(ExpressionParser, Expression_Can_Contain_Basic_Expression_With_Unary_Op_In_The_Beginning)
{
	ExpressionParser p;
	p.parse(" -*");
	const auto& tokens = p.expression().tokens;
	ASSERT_EQ(1u, tokens.size());

	ASSERT_EQ(BuildToken("-", "*", {}), tokens.front());
}

TEST(ExpressionParser, Expression_With_Only_Unary_Op_Will_Throw_InvalidExpression)
{
	ExpressionParser p;

	ASSERT_THROW({
		p.parse("+");
	}, InvalidExpression);
}

TEST(ExpressionParser, Differentiate_Current_Address_Symbol_From_Multiply_Binary_Operation)
{
	ExpressionParser p;
	p.parse("***");
	const auto& tokens = p.expression().tokens;
	ASSERT_EQ(2u, tokens.size());

	ASSERT_EQ(BuildToken({}, "*", "*"), tokens[0]);
	ASSERT_EQ(BuildToken("*"), tokens[1]);
}

TEST(ExpressionParser, Full_Token_Is_Unary_Op_With_Basic_Expression_And_Binary_Op)
{
	ExpressionParser p;
	p.parse("+ * - 3");
	const auto& tokens = p.expression().tokens;
	ASSERT_EQ(2u, tokens.size());

	ASSERT_EQ(BuildToken("+", "*", "-"), tokens[0]);
}

TEST(ExpressionParser, Parses_All_Token_To_The_Vector_With_Left_To_Right_Order)
{
	ExpressionParser p;
	//           1 |2|   3| 4|
	p.parse("  -1 + 5*20 / 6  ");
	const auto& tokens = p.expression().tokens;
	ASSERT_EQ(4u, tokens.size());

	ASSERT_EQ(BuildToken("-", "1", "+"), tokens[0]);
	ASSERT_EQ(BuildToken({}, "5", "*"), tokens[1]);
	ASSERT_EQ(BuildToken({}, "20", "/"), tokens[2]);
	ASSERT_EQ(BuildToken("6"), tokens[3]);
}

TEST(ExpressionParser, Parses_Mix_Field_Specification_As_Binary_Op)
{
	ExpressionParser p;
	p.parse("1:3");
	const auto& tokens = p.expression().tokens;
	ASSERT_EQ(2u, tokens.size());

	ASSERT_EQ(BuildToken({}, "1", ":"), tokens[0]);
	ASSERT_EQ(BuildToken("3"), tokens[1]);
}

TEST(ExpressionParser, Parses_Special_MIXAL_Binary_Operations)
{
	ExpressionParser p;
	p.parse("1 // 3");
	const auto& tokens = p.expression().tokens;
	ASSERT_EQ(2u, tokens.size());

	ASSERT_EQ(BuildToken({}, "1", "//"), tokens[0]);
	ASSERT_EQ(BuildToken("3"), tokens[1]);
}

TEST(ExpressionParser, Fails_To_Parse_Expressions_With_Too_Long_Symbols)
{
	ExpressionParser p;
	auto long_symbol = MakeTooLongSymbol();

	ASSERT_THROW({
		p.parse(long_symbol);
	}, InvalidExpression);
}
