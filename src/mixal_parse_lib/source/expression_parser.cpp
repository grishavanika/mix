#include <mixal_parse/expression_parser.h>

#include <core/string.h>

#include <algorithm>

#include <cassert>
#include <cctype>

using namespace mixal_parse;

namespace
{

bool IsValid(const ExpressionToken& token)
{
	return token.basic_expr.is_valid();
}

bool IsValid(const Expression& expr)
{
	return !expr.tokens.empty() &&
		!expr.tokens.back().binary_op;
}

class InvalidExpression :
	public std::logic_error
{
	using std::logic_error::logic_error;
};

} // namespace

std::size_t ExpressionParser::do_parse_stream(std::string_view str, std::size_t offset)
{
	parse_str_ = str;
	parse_pos_ = offset;

	try
	{
		parse_basic_expr_with_optional_unary_op();

		while (!eof())
		{
			parse_basic_expr_with_binary_op();
		}
	}
	catch (const InvalidExpression&)
	{
	}

	if (IsValid(current_token_))
	{
		finish_current_token();
	}

	return IsValid(expression_)
		? last_parsed_token_pos_
		: InvalidStreamPosition();
}

const Expression& ExpressionParser::expression() const
{
	return expression_;
}

void ExpressionParser::do_clear()
{
	expression_ = {};
	parse_str_ = {};
	parse_pos_ = 0;
	current_token_ = ExpressionToken{};
	last_parsed_token_pos_ = 0;
}

void ExpressionParser::parse_basic_expr_with_optional_unary_op()
{
	skip_white_spaces();

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
	if ((parse_pos_ != 0) &&
		!std::isspace(parse_str_[parse_pos_ - 1]))
	{
		--parse_pos_;
		return;
	}

	throw_error("Invalid Parser's state");
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
	assert(IsValid(current_token_));
	expression_.tokens.push_back(std::move(current_token_));
	current_token_ = ExpressionToken{};
}
