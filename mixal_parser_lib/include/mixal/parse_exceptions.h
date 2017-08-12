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

class InvalidLabel :
	public std::logic_error
{
public:
	InvalidLabel()
		: std::logic_error{"Invalid label"}
	{
	}
};

class InvalidLine :
	public std::logic_error
{
public:
	InvalidLine()
		: std::logic_error{"Invalid line"}
	{
	}
};

} // namespace mixal

