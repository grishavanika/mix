#pragma once
#include <mixal_parse/config.h>

#include <core/string.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT Text
{
public:
	Text(const std::string_view& str);
	Text(const char* str);

	std::string_view data() const;

private:
	std::string_view str_;
};

bool operator==(const Text& lhs, const Text& rhs);

} // namespace mixal_parse

////////////////////////////////////////////////////////////////////////////////

namespace mixal_parse {

inline Text::Text(const std::string_view& str)
	: str_{str}
{
}

inline Text::Text(const char* str)
	: str_{str}
{
}

inline std::string_view Text::data() const
{
	return str_;
}

inline bool operator==(const Text& lhs, const Text& rhs)
{
	return (lhs.data() == rhs.data());
}

} // namespace mixal_parse

