#include <mixal_parse/field_parser.h>
#include <mixal_parse/expression_parser.h>

#include <core/string.h>

using namespace mixal_parse;

std::size_t FieldParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	const auto first_char_pos = SkipLeftWhiteSpaces(str, offset);
	if (first_char_pos == str.size())
	{
		// Empty Field
		return str.size();
	}

	if (str[first_char_pos] != '(')
	{
		// Assume, we parsed "Empty Field"
		return first_char_pos;
	}

	ExpressionParser expr_parser;
	const auto expr_end = expr_parser.parse_stream(str, first_char_pos + 1);
	if (IsInvalidStreamPosition(expr_end))
	{
		return InvalidStreamPosition();
	}

	const auto first_char_after_expr = ExpectFirstNonWhiteSpaceChar(')', str, expr_end);
	if (first_char_after_expr == str.size())
	{
		return InvalidStreamPosition();
	}

	field_ = expr_parser.expression();

	return (first_char_after_expr + 1);
}

void FieldParser::do_clear()
{
	field_ = {};
}

const Field& FieldParser::field() const
{
	return field_;
}

