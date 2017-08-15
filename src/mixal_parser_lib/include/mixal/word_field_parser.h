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
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

private:
	std::optional<Expression> expression_;
};

} // namespace mixal

