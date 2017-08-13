#include <mixal/expression_parser.h>
#include <mixal/parse_exceptions.h>

#include <core/string.h>

#include <algorithm>

#include <cassert>
#include <cctype>

using namespace mixal;

void ExpressionParser::parse(std::string_view str)
{
	clear();
	
	parse_str_ = core::Trim(str);
	
	parse_basic_expr_with_optional_unary_op();

	while (!eof())
	{
		parse_basic_expr_with_binary_op();
	}

	swap(tokens_, final_tokens_);
}

std::string_view ExpressionParser::str() const
{
	return parse_str_;
}

const std::vector<ExpressionParser::Token>& ExpressionParser::tokens() const
{
	return final_tokens_;
}

void ExpressionParser::clear()
{
	tokens_.clear();
	final_tokens_.clear();
	parse_str_ = {};
	parse_pos_ = 0;
	current_token_ = Token{};
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
	tokens_.push_back(std::move(current_token_));
	current_token_ = Token{};
}

namespace mixal {

bool operator==(const ExpressionParser::Token& lhs, const ExpressionParser::Token& rhs)
{
	return (lhs.unary_op == rhs.unary_op) &&
		(lhs.basic_expr == rhs.basic_expr) &&
		(lhs.binary_op == rhs.binary_op);
}

} // namespace mixal
