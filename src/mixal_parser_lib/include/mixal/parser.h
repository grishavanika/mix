#pragma once
#include <core/string.h>

namespace mixal {

class IParser
{
public:
	std::size_t parse_stream(std::string_view str, std::size_t offset = 0)
	{
		do_clear();

		if (offset > str.size())
		{
			return str.npos;
		}

		const auto pos = do_parse_stream(str, offset);
		if (pos == str.npos)
		{
			do_clear();
		}

		return pos;
	}

protected:
	~IParser() = default;

	virtual void do_clear() = 0;
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) = 0;
};

} // namespace mixal

