#pragma once
#include <mixal/parser.h>
#include <mixal/parsers_utils.h>

#include <core/optional.h>

namespace mixal {

class LabelParser final :
	public IParser
{
public:
	bool empty() const;
	std::string_view name() const;
	bool is_local_symbol() const;
	std::optional<LocalSymbolId> local_symbol_id() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

	bool parse_local_symbol();

private:
	std::optional<LocalSymbolId> local_symbol_id_;
	std::string_view name_;
};

} // namespace mixal

