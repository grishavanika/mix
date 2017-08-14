#pragma once
#include <mixal/parser.h>
#include <mixal/expression.h>

namespace mixal {

class WordFieldParser final :
	public IParser
{
public:
	bool empty() const;
	std::optional<Expression> expression();

private:
	virtual void do_parse(std::string_view str) override;
	virtual void do_clear() override;

private:
	std::optional<Expression> expression_;
};

} // namespace mixal

