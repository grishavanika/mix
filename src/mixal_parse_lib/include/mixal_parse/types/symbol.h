#pragma once
#include <mixal_parse/parsers_utils.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT Symbol
{
public:
	enum class Kind
	{
		Usual,
		Here,
		Forward,
		Backward,
	};

public:
	Symbol();
	Symbol(const std::string_view& name,
		Kind kind = Kind::Usual,
		LocalSymbolId id = k_invalid_local_symbol_id);

	bool is_valid() const;
	std::string_view name() const;

	Kind kind() const;
	bool is_local() const;

	LocalSymbolId local_id() const;

private:
	std::string_view name_;
	Kind kind_;
	LocalSymbolId id_;
};

bool operator==(const Symbol& lhs, const Symbol& rhs);
bool operator<(const Symbol& lhs, const Symbol& rhs);

} // namespace mixal_parse

////////////////////////////////////////////////////////////////////////////////

namespace mixal_parse {

inline Symbol::Symbol()
	: Symbol{{}}
{
}

inline Symbol::Symbol(const std::string_view& name,
	Kind kind /*= Kind::Usual*/,
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

inline Symbol::Kind Symbol::kind() const
{
	return kind_;
}

inline bool Symbol::is_local() const
{
	return (kind_ != Kind::Usual);
}

inline LocalSymbolId Symbol::local_id() const
{
	return id_;
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

