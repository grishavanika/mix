#pragma once
#include <mixal/parser.h>
#include <mixal/expression.h>

namespace mixal {

class WordFieldParser final :
	public IParser
{
public:
	virtual void parse(std::string_view str) override;

	bool empty() const;
	std::optional<Expression> expression();

private:
	void clear();

private:
	std::optional<Expression> expression_;
};

} // namespace mixal

