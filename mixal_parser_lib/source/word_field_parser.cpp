#include <mixal/word_field_parser.h>
#include <mixal/parse_exceptions.h>
#include <mixal/expression_parser.h>

#include <core/string.h>

using namespace mixal;

void WordFieldParser::parse(std::string_view str)
{
	clear();

	auto field_str = core::Trim(str);
	if (field_str.empty())
	{
		return;
	}

	if (field_str.size() < 2)
	{
		throw InvalidField{};
	}

	if ((field_str.front() != '(') ||
		(field_str.back() != ')'))
	{
		throw InvalidField{};
	}

	auto expr_str = field_str;
	expr_str.remove_prefix(1);
	expr_str.remove_suffix(1);

	ExpressionParser expr_parser;
	expr_parser.parse(expr_str);

	expression_ = expr_parser.expression();
}

void WordFieldParser::clear()
{
	expression_.reset();
}

bool WordFieldParser::empty() const
{
	return !expression_.has_value();
}

std::optional<Expression> WordFieldParser::expression()
{
	return expression_;
}

