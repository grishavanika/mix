#pragma once
#include <mixal_parse/types/expression.h>

namespace mixal_parse {

class Index
{
public:
	Index();
	Index(const Expression& expr);

	bool empty() const;

	const Expression& expression() const;

private:
	Expression expr_;
};

} // namespace mixal_parse


namespace mixal_parse {

inline Index::Index()
	: expr_{}
{
}

inline Index::Index(const Expression& expr)
	: expr_{expr}
{
}

inline bool Index::empty() const
{
	return !expr_.is_valid();
}

inline const Expression& Index::expression() const
{
	return expr_;
}

} // namespace mixal_parse
