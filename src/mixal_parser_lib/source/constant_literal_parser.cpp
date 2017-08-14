#include <mixal/constant_literal_parser.h>
#include <mixal/parse_exceptions.h>
#include <mixal/constant_word_expression_parser.h>

#include <core/string.h>

using namespace mixal;

void ConstantLiteralParser::do_parse(std::string_view str)
{
	auto literal_str = core::Trim(str);

	if (literal_str.size() < 2)
	{
		throw InvalidLiteral{};
	}

	if ((literal_str.front() != '=') ||
		(literal_str.back() != '='))
	{
		throw InvalidLiteral{};
	}

	literal_str.remove_prefix(1);
	literal_str.remove_suffix(1);

	if (literal_str.size() > k_max_symbol_length)
	{
		throw InvalidLiteral{};
	}

	ConstantWordExpressionParser parser;
	parser.parse(literal_str);

	expression_ = parser.expression();

}

void ConstantLiteralParser::do_clear()
{
	expression_ = {};
}

const WordExpression& ConstantLiteralParser::expression() const
{
	return expression_;
}

