#pragma once
#include <mixal_parse/types/expression.h>

namespace mixal_parse {

class Field
{
public:
	Field();
	Field(const Expression& expr);

	bool empty() const;

	const Expression& expression() const;

private:
	Expression expr_;
};

} // namespace mixal_parse


namespace mixal_parse {

inline Field::Field()
	: expr_{}
{
}

inline Field::Field(const Expression& expr)
	: expr_{expr}
{
}

inline bool Field::empty() const
{
	return !expr_.is_valid();
}

inline const Expression& Field::expression() const
{
	return expr_;
}

} // namespace mixal_parse
