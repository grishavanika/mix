#pragma once
#include <string_view>

namespace mixal {

class IParser
{
public:
	virtual void parse(std::string_view str) = 0;
	virtual std::string_view str() const = 0;

protected:
	~IParser() = default;
};

} // namespace mixal

