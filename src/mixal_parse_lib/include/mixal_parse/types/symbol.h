#pragma once
#include <mixal_parse/parsers_utils.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT Symbol
{
public:
	Symbol();

	Symbol(const char* name,
		LocalSymbolKind kind = LocalSymbolKind::Usual,
		LocalSymbolId id = k_invalid_local_symbol_id);

	Symbol(const std::string_view& name,
		LocalSymbolKind kind = LocalSymbolKind::Usual,
		LocalSymbolId id = k_invalid_local_symbol_id);
	
	static Symbol FromString(const std::string_view& str);

	bool is_valid() const;
	std::string_view name() const;

	LocalSymbolKind kind() const;
	bool is_local() const;

	LocalSymbolId local_id() const;

private:
	std::string_view name_;
	LocalSymbolKind kind_;
	LocalSymbolId id_;
};

bool operator==(const Symbol& lhs, const Symbol& rhs);
bool operator<(const Symbol& lhs, const Symbol& rhs);

} // namespace mixal_parse

////////////////////////////////////////////////////////////////////////////////

namespace mixal_parse {

inline Symbol::Symbol()
	: Symbol{std::string_view{}}
{
}

inline Symbol::Symbol(const char* name,
	LocalSymbolKind kind /*= LocalSymbolKind::Usual*/,
	LocalSymbolId id /*= k_invalid_local_symbol_id*/)
		: Symbol{std::string_view{name}, kind, id}
{
}

inline Symbol::Symbol(const std::string_view& name,
	LocalSymbolKind kind /*= LocalSymbolKind::Usual*/,
	LocalSymbolId id /*= k_invalid_local_symbol_id*/)
		: name_{name}
		, kind_{kind}
		, id_{id}
{
}

inline bool Symbol::is_valid() const
{
	return IsSymbol(name_);
}

inline std::string_view Symbol::name() const
{
	return name_;
}

inline LocalSymbolKind Symbol::kind() const
{
	return kind_;
}

inline bool Symbol::is_local() const
{
	return (kind_ != LocalSymbolKind::Usual);
}

inline LocalSymbolId Symbol::local_id() const
{
	return id_;
}

inline /*static*/ Symbol Symbol::FromString(const std::string_view& str)
{
	if (!IsSymbol(str))
	{
		return Symbol{};
	}

	if (!IsLocalSymbol(str))
	{
		return Symbol{str};
	}

	const auto kind = ParseLocalSymbolKind(str);
	const auto id = ParseLocalSymbolId(str);
	if ((kind == LocalSymbolKind::Unknown) ||
		!IsValidLocalSymbolId(id))
	{
		return Symbol{};
	}

	return Symbol{str, kind, id};
}

inline bool operator==(const Symbol& lhs, const Symbol& rhs)
{
	return (lhs.name() == rhs.name());
}

inline bool operator<(const Symbol& lhs, const Symbol& rhs)
{
	return (lhs.name() < rhs.name());
}

} // namespace mixal_parse

