#include <mixal/w_value_parser.h>

#include <core/string.h>

using namespace mixal;

std::size_t WValueParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	auto pos = offset;
	auto last_parsed_expr_pos = str.npos;

	while (pos < str.size())
	{
		const auto expr_end = parse_expr_with_field(str, pos);
		if (expr_end == str.npos)
		{
			break;
		}
		last_parsed_expr_pos = expr_end;

		pos = ExpectFirstNonWhiteSpaceChar(',', str, expr_end);
		if (pos != str.size())
		{
			pos = pos + 1;
		}
	}

	return value_.tokens.empty()
		? str.npos
		: last_parsed_expr_pos;
}

std::size_t WValueParser::parse_expr_with_field(std::string_view str, std::size_t offset)
{
	ExpressionParser expr_parser;
	const auto expr_end = expr_parser.parse_stream(str, offset);
	if (expr_end == str.npos)
	{
		return str.npos;
	}

	WordFieldParser field_parser;
	auto field_end = field_parser.parse_stream(str, expr_end);
	if (field_end == str.npos)
	{
		field_end = expr_end;
	}

	add_token(std::move(expr_parser), std::move(field_parser));
	return field_end;
}

void WValueParser::add_token(ExpressionParser&& expr, WordFieldParser&& field)
{
	WValueToken token;
	token.expression = expr.expression();
	token.field = field.expression();

	value_.tokens.push_back(std::move(token));
}

const WValue& WValueParser::value() const
{
	return value_;
}

void WValueParser::do_clear()
{
	value_ = {};
}

