#pragma once
#include <mixal_parse/config.h>

#include <core/string.h>

#include <cstdint>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT Number
{
public:
	Number(const std::string_view& str);

	std::string_view data() const;

	bool is_zero() const;

	// Since it's allowed to put number with 10 digits length,
	// we need at most 34 bits to store maximum possible value.
	std::uint64_t value() const;

private:
	static std::uint64_t ValueFromString(const std::string_view& str);

private:
	std::string_view str_;
	std::uint64_t n_;
};

} // namespace mixal_parse

////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <cassert>

namespace mixal_parse {

inline Number::Number(const std::string_view& str)
	: str_{str}
	, n_{ValueFromString(str)}
{
}

inline std::string_view Number::data() const
{
	return str_;
}

inline bool Number::is_zero() const
{
	return (n_ == 0);
}

inline std::uint64_t Number::value() const
{
	return n_;
}

inline /*static*/ std::uint64_t Number::ValueFromString(const std::string_view& str)
{
	// #TODO: `std::from_chars()`
	std::stringstream stream;
	stream << str;
	std::uint64_t v = 0;
	stream >> v;
	assert(stream);
	return v;
}

} // namespace mixal_parse

