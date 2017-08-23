#pragma once
#include <mixal_parse/types/expression.h>

namespace mixal_parse {

class Address
{
public:
	Address();
	Address(const Expression& expr);
	Address(const WValue& wvalue);

	const Expression& expression() const;
	const WValue& w_value() const;

	bool empty() const;
	bool has_expression() const;
	bool has_w_value() const;
	bool has_literal_constant() const;

private:
	// #TODO: C++17 std::variant<Expression, WValue>
	Expression expr_;
	WValue wvalue_;
};

} // namespace mixal_parse

////////////////////////////////////////////////////////////////////////////////

namespace mixal_parse {

inline Address::Address()
	: expr_{}
	, wvalue_{}
{
}

inline Address::Address(const Expression& expr)
	: expr_{expr}
	, wvalue_{}
{
}

inline Address::Address(const WValue& wvalue)
	: expr_{}
	, wvalue_{wvalue}
{
}

inline const Expression& Address::expression() const
{
	return expr_;
}

inline const WValue& Address::w_value() const
{
	return wvalue_;
}

inline bool Address::empty() const
{
	return !has_expression() && !has_w_value();
}

inline bool Address::has_expression() const
{
	return expr_.is_valid();
}

inline bool Address::has_w_value() const
{
	return wvalue_.is_valid();
}

inline bool Address::has_literal_constant() const
{
	return has_w_value();
}

} // namespace mixal_parse

