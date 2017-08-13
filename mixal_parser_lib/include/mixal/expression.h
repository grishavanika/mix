#pragma once
#include <mixal/parsers_utils.h>

#include <optional>
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

} // namespace mixal



