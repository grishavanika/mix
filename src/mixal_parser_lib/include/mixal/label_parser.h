#pragma once
#include <mixal/parser.h>

#include <core/optional.h>

namespace mixal {

// In range [0; 9]
using LocalSymbolId = unsigned char;

class LabelParser final :
	public IParser
{
public:
	bool empty() const;
	std::string_view name() const;
	bool is_local_symbol() const;
	std::optional<LocalSymbolId> local_symbol_id() const;

private:
	virtual void do_parse(std::string_view str) override;
	virtual void do_clear() override;

private:
	std::optional<LocalSymbolId> local_symbol_id_;
	std::string_view name_;
};

} // namespace mixal

