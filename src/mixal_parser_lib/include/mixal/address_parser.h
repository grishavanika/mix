#pragma once
#include <mixal/parser.h>
#include <mixal/expression.h>

#include <core/optional.h>

namespace mixal {

class MIXAL_PARSER_LIB_EXPORT AddressParser final :
	public ParserBase
{
public:
	bool empty() const;

	ConstOptionalRef<Expression> expression() const;
	ConstOptionalRef<WValue> w_value() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

	std::size_t try_parse_expression(const std::string_view& str, std::size_t offset);
	std::size_t try_parse_literal(const std::string_view& str, std::size_t offset);

private:
	std::optional<Expression> expression_;
	std::optional<WValue> w_value_;
};

} // namespace mixal
