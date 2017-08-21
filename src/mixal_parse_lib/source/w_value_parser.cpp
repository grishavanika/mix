#include <mixal_parse/w_value_parser.h>

#include <core/string.h>

using namespace mixal_parse;

std::size_t WValueParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	auto pos = offset;
	auto last_parsed_expr_pos = InvalidStreamPosition();

	while (pos < str.size())
	{
		const auto expr_end = parse_expr_with_field(str, pos);
		if (IsInvalidStreamPosition(expr_end))
		{
			break;
		}
		last_parsed_expr_pos = expr_end;

		pos = ExpectFirstNonWhiteSpaceChar(',', str, expr_end);
		if (pos != str.size())
		{
			// Skip `,`, try to parse next portion
			pos = pos + 1;
		}
	}

	return !value_.is_valid()
		? InvalidStreamPosition()
		: last_parsed_expr_pos;
}

std::size_t WValueParser::parse_expr_with_field(std::string_view str, std::size_t offset)
{
	ExpressionParser expr_parser;
	const auto expr_end = expr_parser.parse_stream(str, offset);
	if (IsInvalidStreamPosition(expr_end))
	{
		return InvalidStreamPosition();
	}

	FieldParser field_parser;
	auto field_end = field_parser.parse_stream(str, expr_end);
	if (IsInvalidStreamPosition(field_end))
	{
		field_end = expr_end;
	}

	add_token(std::move(expr_parser), std::move(field_parser));
	return field_end;
}

void WValueParser::add_token(ExpressionParser&& expr, FieldParser&& field)
{
	WValue::Token token;
	token.expression = expr.expression();
	token.field = field.expression();

	value_.add_token(std::move(token));
}

const WValue& WValueParser::value() const
{
	return value_;
}

void WValueParser::do_clear()
{
	value_ = {};
}

