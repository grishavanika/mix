#include <mixal/constant_literal_parser.h>
#include <mixal/parse_exceptions.h>
#include <mixal/constant_word_expression_parser.h>

#include <core/string.h>

using namespace mixal;

std::size_t ConstantLiteralParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	const auto first_char_pos = ExpectFirstNonWhiteSpaceChar('=', str, offset);
	if (first_char_pos == str.size())
	{
		return str.npos;
	}

	ConstantWordExpressionParser expr_parser;
	const auto expr_end = expr_parser.parse_stream(str, first_char_pos + 1);
	if (expr_end == str.npos)
	{
		return str.npos;
	}

	const auto first_char_after_expr = ExpectFirstNonWhiteSpaceChar('=', str, expr_end);
	if (first_char_after_expr == str.size())
	{
		return str.npos;
	}

	const auto expression_length = (first_char_after_expr - first_char_pos - 1);
	if (expression_length > k_max_symbol_length)
	{
		return str.npos;
	}

	expression_ = expr_parser.expression();

	return (first_char_after_expr + 1);
}

void ConstantLiteralParser::do_clear()
{
	expression_ = {};
}

const WordExpression& ConstantLiteralParser::expression() const
{
	return expression_;
}

