#pragma once
#include <core/string.h>

#include <mixal/parse_exceptions.h>

namespace mixal {

class IParser
{
public:
	void parse(std::string_view str)
	{
		if (!parse_stream(str))
		{
			throw ParseError{"Something went wrong. This code will be removed"};
		}
	}

	bool parse_stream(std::string_view& str)
	{
		do_clear();

		if (!do_parse_stream(str))
		{
			do_clear();
			return false;
		}

		return true;
	}

protected:
	~IParser() = default;

private:
	virtual void do_parse(std::string_view /*str*/)
	{
		throw ParseError{"Deprecated function. Will be removed"};
	}

	virtual void do_clear() = 0;

	virtual bool do_parse_stream(std::string_view& str)
	{
		try
		{
			do_parse(str);
		}
		catch (const ParseError&)
		{
			return false;
		}
		return true;
	}
};

} // namespace mixal

