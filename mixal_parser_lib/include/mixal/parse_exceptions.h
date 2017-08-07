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

class UnknownOperationId :
	public std::logic_error
{
public:
	UnknownOperationId()
		: std::logic_error{"Unknown operation id"}
	{
	}
};

} // namespace mixal

