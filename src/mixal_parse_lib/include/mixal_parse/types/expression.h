#pragma once
#include <mixal_parse/types/basic_expression.h>

#include <core/optional.h>

#include <vector>

namespace mixal_parse {

class Expression
{
public:
	struct Token
	{
		std::optional<UnaryOperation> unary_op;
		BasicExpression basic_expr;
		std::optional<BinaryOperation> binary_op;
	};

	const std::vector<Token>& tokens() const;

	void add_token(Token&& token);

	bool is_valid() const;

private:
	std::vector<Token> tokens_;
};

struct WValueToken
{
	Expression expression;
	std::optional<Expression> field;
};

struct WValue
{
	std::vector<WValueToken> tokens;
};

inline const std::vector<Expression::Token>& Expression::tokens() const
{
	return tokens_;
}

inline void Expression::add_token(Token&& token)
{
	// Only first token _can_ has (optional) unary operation
	assert(tokens_.empty() || !token.unary_op);
	// Previous token (if any) should has binary operation since
	// current token to append becomes right hand side operand
	assert(tokens_.empty() || tokens_.back().binary_op);

	tokens_.push_back(std::move(token));
}

inline bool Expression::is_valid() const
{
	// Last token should not has binary operation since then
	// it will mean that we expect to append another "basic expression" as
	// (binary) operator right hand side operand
	return !tokens_.empty() &&
		!tokens_.back().binary_op;
}

inline bool operator==(const Expression::Token& lhs, const Expression::Token& rhs)
{
	return (lhs.unary_op == rhs.unary_op) &&
		(lhs.basic_expr == rhs.basic_expr) &&
		(lhs.binary_op == rhs.binary_op);
}

inline bool operator==(const Expression& lhs, const Expression& rhs)
{
	return (lhs.tokens() == rhs.tokens());
}

inline bool operator==(const WValueToken& lhs, const WValueToken& rhs)
{
	return (lhs.expression == rhs.expression) &&
		(lhs.field == rhs.field);
}

inline bool operator==(const WValue& lhs, const WValue& rhs)
{
	return (lhs.tokens == rhs.tokens);
}

} // namespace mixal_parse



