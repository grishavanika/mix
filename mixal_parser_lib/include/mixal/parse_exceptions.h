#pragma once
#include <stdexcept>

namespace mixal {

class NotImplemented :
	public std::logic_error
{
public:
	NotImplemented()
		: std::logic_error{"Not implemented"}
	{
	}

};

} // namespace mixal

