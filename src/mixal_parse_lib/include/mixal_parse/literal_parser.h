#pragma once
#include <mixal_parse/parser_base.h>
#include <mixal_parse/expression.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT LiteralParser final :
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

} // namespace mixal_parse

