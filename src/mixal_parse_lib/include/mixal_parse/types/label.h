#pragma once
#include <mixal_parse/types/symbol.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT Label
{
public:
	Label();
	Label(const Symbol& symbol);

	bool empty() const;
	std::string_view name() const;
	LocalSymbolId local_id() const;
	bool is_local_symbol() const;

private:
	Symbol symbol_;
};

} // namespace mixal_parse

////////////////////////////////////////////////////////////////////////////////

namespace mixal_parse {

inline Label::Label()
	: Label{{}}
{
}

inline Label::Label(const Symbol& symbol)
	: symbol_{symbol}
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
	return symbol_.local_id();
}

inline bool Label::is_local_symbol() const
{
	return symbol_.is_local();
}

} // namespace mixal_parse
