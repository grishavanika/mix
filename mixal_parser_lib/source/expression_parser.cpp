#include <mixal/expression_parser.h>
#include <mixal/parse_exceptions.h>

#include <core/string.h>

#include <algorithm>

#include <cassert>
#include <cctype>

using namespace mixal;

void ExpressionParser::do_parse(std::string_view str)
{
	parse_str_ = core::Trim(str);
	
	parse_basic_expr_with_optional_unary_op();

	while (!eof())
	{
		parse_basic_expr_with_binary_op();
	}

	std::swap(expression_, final_expression_);
}

const Expression& ExpressionParser::expression() const
{
	return final_expression_;
}

void ExpressionParser::do_clear()
{
	expression_ = {};
	final_expression_ = {};
	parse_str_ = {};
	parse_pos_ = 0;
	current_token_ = ExpressionToken{};
}

void ExpressionParser::parse_basic_expr_with_optional_unary_op()
{
	const auto ch = current_char();
	if (IsUnaryOperationBegin(ch))
	{
		return parse_basic_expr_with_unary_op();
	}
	else if (IsBasicExpressionBegin(ch))
	{
		return parse_single_basic_expr();
	}

	throw_error("Expecting <Unary operation> OR <Basic Expression>");
}

void ExpressionParser::parse_basic_expr_with_unary_op()
{
	current_token_.unary_op = build_non_empty_expr(&IsUnaryOperationChar, &IsCompletedUnaryOperation);
	parse_single_basic_expr();
}

void ExpressionParser::parse_basic_expr_with_binary_op()
{
	current_token_.binary_op = build_non_empty_expr(&IsBinaryOperationChar, &IsCompletedBinaryOperation);

	finish_current_token();

	parse_single_basic_expr();
}

void ExpressionParser::parse_single_basic_expr()
{
	current_token_.basic_expr = build_non_empty_expr(&IsBasicExpressionChar, &IsCompletedBasicExpression);
	
	skip_white_spaces();
	if (eof())
	{
		finish_current_token();
	}
}

bool ExpressionParser::eof() const
{
	return (parse_pos_ >= parse_str_.size());
}

char ExpressionParser::move_to_next_char()
{
	skip_white_spaces();

	const auto ch = current_char();
	++parse_pos_;
	return ch;
}

void ExpressionParser::put_char_back()
{
	assert(parse_pos_ != 0);
	--parse_pos_;
	assert(!std::isspace(current_char()));
}

void ExpressionParser::skip_white_spaces()
{
	while (!eof() && std::isspace(parse_str_[parse_pos_]))
	{
		++parse_pos_;
	}
}

char ExpressionParser::current_char() const
{
	if (eof())
	{
		throw_error("Unexpected end of expression");
	}

	return parse_str_[parse_pos_];
}

void ExpressionParser::throw_error(const char* details) const
{
	// #TODO: more useful exception
	throw InvalidExpression{details};
}

void ExpressionParser::finish_current_token()
{
	expression_.tokens.push_back(std::move(current_token_));
	current_token_ = ExpressionToken{};
}
