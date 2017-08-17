#include <mixal_parse/address_parser.h>
#include <mixal_parse/expression_parser.h>
#include <mixal_parse/literal_parser.h>
#include <mixal_parse/parsers_utils.h>

#include <core/string.h>

using namespace mixal_parse;

std::size_t AddressParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	const auto first_char_pos = SkipLeftWhiteSpaces(str, offset);
	if (first_char_pos == str.size())
	{
		// Empty
		return first_char_pos;
	}
	
	const auto expr_end = try_parse_expression(str, first_char_pos);
	if (!IsInvalidStreamPosition(expr_end))
	{
		return expr_end;
	}

	const auto literal_end = try_parse_literal(str, first_char_pos);
	if (!IsInvalidStreamPosition(literal_end))
	{
		return literal_end;
	}

	// Assume we are empty
	return first_char_pos;
}

std::size_t AddressParser::try_parse_expression(const std::string_view& str, std::size_t offset)
{
	ExpressionParser parser;
	const auto end = parser.parse_stream(str, offset);
	if (!IsInvalidStreamPosition(end))
	{
		expression_ = parser.expression();
	}
	
	return end;
}

std::size_t AddressParser::try_parse_literal(const std::string_view& str, std::size_t offset)
{
	LiteralParser parser;
	const auto end = parser.parse_stream(str, offset);
	if (!IsInvalidStreamPosition(end))
	{
		w_value_ = parser.value();
	}

	return end;
}

void AddressParser::do_clear()
{
	expression_ = std::nullopt;
	w_value_ = std::nullopt;
}

ConstOptionalRef<Expression> AddressParser::expression() const
{
	return expression_;
}

ConstOptionalRef<WValue> AddressParser::w_value() const
{
	return w_value_;
}

bool AddressParser::empty() const
{
	return !expression_ && !w_value_;
}
