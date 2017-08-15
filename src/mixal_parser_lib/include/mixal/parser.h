#pragma once
#include <core/string.h>

#include <mixal/parse_exceptions.h>

namespace mixal {

class IParser
{
public:
	void parse(std::string_view str)
	{
		if (parse_stream(str) == str.npos)
		{
			throw ParseError{"Something went wrong. This code will be removed"};
		}
	}

	std::size_t parse_stream(std::string_view str, std::size_t offset = 0)
	{
		do_clear();

		// #TODO: validate that `offset <= str.size()`
		const auto pos = do_parse_stream(str, offset);

		if (pos == str.npos)
		{
			do_clear();
		}

		return pos;
	}

protected:
	~IParser() = default;

private:
	virtual void do_parse(std::string_view /*str*/)
	{
		throw ParseError{"Deprecated function. Will be removed"};
	}

	virtual void do_clear() = 0;

	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset)
	{
		try
		{
			do_parse(str.substr(offset));
		}
		catch (const ParseError&)
		{
			return str.npos;
		}
		
		return str.size();
	}
};

} // namespace mixal

