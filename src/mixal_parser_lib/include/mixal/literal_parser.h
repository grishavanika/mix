#pragma once
#include <mixal/parser_base.h>
#include <mixal/expression.h>

namespace mixal {

class MIXAL_PARSER_LIB_EXPORT LiteralParser final :
	public ParserBase
{
public:
	const WValue& value() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

private:
	WValue value_;
};

} // namespace mixal

