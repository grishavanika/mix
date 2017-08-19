#pragma once
#include <mixal/types.h>

#include <stdexcept>

namespace mixal {

class MixalError :
	public std::logic_error
{
public:
	using std::logic_error::logic_error;
};

class TooBigWordValueError :
	public MixalError
{
public:
	TooBigWordValueError(const Number& /*n*/)
		: MixalError{"TooBigWordValueError"}
	{
	}
};

class InvalidALFText :
	public MixalError
{
public:
	InvalidALFText(const Text& /*text*/)
		: MixalError{"InvalidALFText"}
	{
	}
};

class SymbolAlreadyDefinedError :
	public MixalError
{
public:
	SymbolAlreadyDefinedError(const Symbol& /*symbol*/, const Word& /*value*/)
		: MixalError{"SymbolAlreadyDefinedError"}
	{
	}
};

class SymbolIsNotDefinedError :
	public MixalError
{
public:
	SymbolIsNotDefinedError(const Symbol& /*symbol*/)
		: MixalError{"SymbolIsNotDefinedError"}
	{
	}
};

} // namespace mixal

