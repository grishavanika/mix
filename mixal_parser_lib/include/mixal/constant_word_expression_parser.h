#pragma once
#include <mixal/parser.h>
#include <mixal/expression.h>

namespace mixal {

class ConstantWordExpressionParser :
	public IParser
{
public:
	virtual void parse(std::string_view str) override;

	const WordExpression& expression() const;

private:
	void clear();
	void parse_single_word_expr(std::string_view str);
	Expression parse_expr(std::string_view str);
	std::optional<Expression> parse_field_expr(std::string_view str);

private:
	WordExpression expression_;
	WordExpression final_expression_;
};

} // namespace mixal



