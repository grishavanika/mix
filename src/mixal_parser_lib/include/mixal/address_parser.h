#pragma once
#include <mixal/expression_parser.h>
#include <mixal/literal_parser.h>

#include <core/optional.h>

namespace mixal {

class MIXAL_PARSER_LIB_EXPORT AddressParser final :
	public IParser
{
public:
	bool empty() const;

	const Expression* expression() const;
	const WValue* w_value() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

	std::size_t try_parse_expression(const std::string_view& str, std::size_t offset);
	std::size_t try_parse_literal(const std::string_view& str, std::size_t offset);

private:
	std::optional<ExpressionParser> expression_parser_;
	std::optional<LiteralParser> literal_parser_;
};

} // namespace mixal
