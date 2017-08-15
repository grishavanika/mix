#include <mixal/word_field_parser.h>
#include <mixal/parse_exceptions.h>
#include <mixal/expression_parser.h>

#include <core/string.h>

#include <cassert>

using namespace mixal;

std::size_t WordFieldParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	const auto first_char_pos = SkipLeftWhiteSpaces(str, offset);
	if (first_char_pos == str.size())
	{
		// Empty `WordField` is valid
		return str.size();
	}

	if (str[first_char_pos] != '(')
	{
		return str.npos;
	}

	ExpressionParser expr_parser;
	const auto expr_end = expr_parser.parse_stream(str, first_char_pos + 1);
	if (expr_end == str.npos)
	{
		return str.npos;
	}

	const auto first_char_after_expr = ExpectFirstNonWhiteSpaceChar(')', str, expr_end);
	if (first_char_after_expr == str.size())
	{
		return str.npos;
	}

	expression_ = expr_parser.expression();

	return (first_char_after_expr + 1);
}

void WordFieldParser::do_clear()
{
	expression_ = std::nullopt;
}

bool WordFieldParser::empty() const
{
	return !expression_;
}

std::optional<Expression> WordFieldParser::expression()
{
	return expression_;
}

