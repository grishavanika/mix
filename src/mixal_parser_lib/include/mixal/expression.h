#pragma once
#include <mixal/parsers_utils.h>

#include <core/optional.h>
#include <vector>

namespace mixal {

struct ExpressionToken
{
	std::optional<UnaryOperation> unary_op;
	BasicExpression basic_expr;
	std::optional<BinaryOperation> binary_op;
};

struct Expression
{
	std::vector<ExpressionToken> tokens;
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

inline bool operator==(const ExpressionToken& lhs, const ExpressionToken& rhs)
{
	return (lhs.unary_op == rhs.unary_op) &&
		(lhs.basic_expr == rhs.basic_expr) &&
		(lhs.binary_op == rhs.binary_op);
}

inline bool operator==(const Expression& lhs, const Expression& rhs)
{
	return (lhs.tokens == rhs.tokens);
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

} // namespace mixal



