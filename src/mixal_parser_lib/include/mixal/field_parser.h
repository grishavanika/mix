#pragma once
#include <mixal/parser.h>
#include <mixal/expression.h>

namespace mixal {

class MIXAL_PARSER_LIB_EXPORT FieldParser final :
	public ParserBase
{
public:
	bool empty() const;
	std::optional<Expression> expression() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

private:
	std::optional<Expression> expression_;
};

} // namespace mixal

