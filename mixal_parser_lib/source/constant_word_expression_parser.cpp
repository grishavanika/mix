#include <mixal/constant_word_expression_parser.h>
#include <mixal/expression_parser.h>
#include <mixal/word_field_parser.h>

#include <core/string.h>

using namespace mixal;

void ConstantWordExpressionParser::do_parse(std::string_view str)
{
	auto expressions = core::Split(str, ',');

	for (auto expr : expressions)
	{
		parse_single_word_expr(core::Trim(expr));
	}

	std::swap(expression_, final_expression_);
}

void ConstantWordExpressionParser::parse_single_word_expr(std::string_view str)
{
	auto field_begin = str.find('(');
	if (field_begin == str.npos)
	{
		field_begin = str.size();
	}

	std::string_view expr_str{str.data(), field_begin};
	std::string_view field_str{str.data() + field_begin, str.size() - field_begin};
	
	WordExpressionToken token;
	token.expression = parse_expr(expr_str);
	token.field = parse_field_expr(field_str);
	expression_.tokens.push_back(std::move(token));
}

Expression ConstantWordExpressionParser::parse_expr(std::string_view str)
{
	ExpressionParser parser;
	parser.parse(str);
	return parser.expression();
}

std::optional<Expression> ConstantWordExpressionParser::parse_field_expr(std::string_view str)
{
	WordFieldParser parser;
	parser.parse(str);
	return parser.expression();
}

const WordExpression& ConstantWordExpressionParser::expression() const
{
	return final_expression_;
}

void ConstantWordExpressionParser::do_clear()
{
	expression_ = {};
	final_expression_ = {};
}

