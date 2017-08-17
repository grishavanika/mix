#pragma once
#include <mixal_parse/parser_base.h>
#include <mixal_parse/parsers_utils.h>

#include <core/optional.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT LabelParser final :
	public ParserBase
{
public:
	bool empty() const;
	std::string_view name() const;
	bool is_local_symbol() const;
	ConstOptionalRef<LocalSymbolId> local_symbol_id() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

	bool parse_local_symbol();

private:
	std::optional<LocalSymbolId> local_symbol_id_;
	std::string_view name_;
};

} // namespace mixal_parse

