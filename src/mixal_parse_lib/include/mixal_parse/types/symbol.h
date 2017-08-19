#pragma once
#include <mixal_parse/parsers_utils.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT Symbol
{
public:
	Symbol();
	Symbol(const std::string_view& name);

	bool is_valid() const;
	std::string_view name() const;

private:
	std::string_view name_;
};

} // namespace mixal_parse

////////////////////////////////////////////////////////////////////////////////

namespace mixal_parse {

inline Symbol::Symbol()
	: Symbol{{}}
{
}

inline Symbol::Symbol(const std::string_view& name)
	: name_{name}
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

} // namespace mixal_parse

