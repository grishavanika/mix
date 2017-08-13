#pragma once
#include <stdexcept>
#include <string>

namespace mixal {

class ParseError :
	public std::logic_error
{
public:
	using std::logic_error::logic_error;
};


class NotImplemented :
	public ParseError
{
public:
	NotImplemented()
		: ParseError{"Not implemented"}
	{
	}
};

class UnknownOperationId :
	public ParseError
{
public:
	UnknownOperationId()
		: ParseError{"Unknown operation id"}
	{
	}
};

class InvalidLabel :
	public ParseError
{
public:
	InvalidLabel()
		: ParseError{"Invalid label"}
	{
	}
};

class InvalidLine :
	public ParseError
{
public:
	InvalidLine()
		: ParseError{"Invalid line"}
	{
	}
};

class InvalidExpression :
	public ParseError
{
public:
	InvalidExpression(const char* details)
		: ParseError{std::string{"Invalid Expression: "} +details}
	{
	}
};

class InvalidField :
	public ParseError
{
public:
	InvalidField()
		: ParseError{"Invalid Field"}
	{
	}
};

class InvalidLiteral :
	public ParseError
{
public:
	InvalidLiteral()
		: ParseError{"Invalid Literal"}
	{
	}
};

} // namespace mixal

