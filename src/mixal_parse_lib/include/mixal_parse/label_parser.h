#pragma once
#include <mixal_parse/parser_base.h>
#include <mixal_parse/types/label.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT LabelParser final :
	public ParserBase
{
public:
	const Label& label() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

	LocalSymbolId parse_local_symbol(const std::string_view& str);

private:
	Label label_;
};

} // namespace mixal_parse

