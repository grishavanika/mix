#pragma once
#include <stdexcept>

namespace mixal {

// #TODO: use custom basic class that will separate
// all exceptions from "only MIXAL-parser" one

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

class InvalidExpression :
	public std::logic_error
{
public:
	InvalidExpression(const char* details)
		: std::logic_error{std::string{"Invalid Expression: "} +details}
	{
	}
};

} // namespace mixal

