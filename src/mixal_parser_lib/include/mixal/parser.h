#pragma once
#include <mixal/parsers_utils.h>

namespace mixal {

class IParser
{
public:
	std::size_t parse_stream(std::string_view str, std::size_t offset = 0);

protected:
	~IParser() = default;

	virtual void do_clear() = 0;
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) = 0;
};

inline std::size_t IParser::parse_stream(std::string_view str, std::size_t offset /*= 0*/)
{
	do_clear();

	if (offset > str.size())
	{
		return InvalidStreamPosition();
	}

	const auto pos = do_parse_stream(str, offset);
	if (IsInvalidStreamPosition(pos))
	{
		// Parser is in undetermined state.
		// Put back in default state for free
		do_clear();
	}

	return pos;
}

} // namespace mixal

