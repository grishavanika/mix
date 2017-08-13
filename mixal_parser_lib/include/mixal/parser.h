#pragma once
#include <string_view>

namespace mixal {

class IParser
{
public:
	void parse(std::string_view str)
	{
		do_clear();
		do_parse(str);
	}

protected:
	~IParser() = default;

private:
	virtual void do_parse(std::string_view str) = 0;
	virtual void do_clear() = 0;
};

} // namespace mixal

