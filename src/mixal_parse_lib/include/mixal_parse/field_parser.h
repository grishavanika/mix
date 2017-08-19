#pragma once
#include <mixal_parse/parser_base.h>
#include <mixal_parse/types/expression.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT FieldParser final :
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

} // namespace mixal_parse

