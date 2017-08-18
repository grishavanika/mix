#pragma once
#include <mixal_parse/symbol.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT Label
{
public:
	Label();
	Label(const Symbol& symbol);
	Label(const Symbol& symbol, LocalSymbolId id);

	bool empty() const;
	std::string_view name() const;
	LocalSymbolId local_id() const;
	bool is_local_symbol() const;

private:
	Symbol symbol_;
	LocalSymbolId id_;
};

} // namespace mixal_parse

////////////////////////////////////////////////////////////////////////////////

namespace mixal_parse {

inline Label::Label()
	: Label{{}}
{
}

inline Label::Label(const Symbol& symbol)
	: Label{symbol, k_invalid_local_symbol_id}
{
}

inline Label::Label(const Symbol& symbol, LocalSymbolId id)
	: symbol_{symbol}
	, id_{id}
{
}

inline bool Label::empty() const
{
	return !symbol_.is_valid();
}

inline std::string_view Label::name() const
{
	return symbol_.name();
}

inline LocalSymbolId Label::local_id() const
{
	return id_;
}

inline bool Label::is_local_symbol() const
{
	return IsValidLocalSymbolId(id_);
}

} // namespace mixal_parse
