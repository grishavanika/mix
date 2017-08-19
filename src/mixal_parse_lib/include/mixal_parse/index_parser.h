#pragma once
#include <mixal_parse/parser_base.h>
#include <mixal_parse/types/expression.h>

#include <core/optional.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT IndexParser final :
	public ParserBase
{
public:
	bool empty() const;

	ConstOptionalRef<Expression> expression() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

	std::size_t try_parse_expression(const std::string_view& str, std::size_t offset);

private:
	std::optional<Expression> expression_;
};

} // namespace mixal_parse
