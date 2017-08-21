#pragma once
#include <mixal/types.h>

#include <stdexcept>

namespace mixal {

class MixalException :
	public std::logic_error
{
public:
	using Base = std::logic_error;
	using Base::Base;
};

class TooBigWordValueError :
	public MixalException
{
public:
	TooBigWordValueError(const Number& /*n*/)
		: MixalException{"too big word value"}
	{
	}
};

class InvalidALFText :
	public MixalException
{
public:
	InvalidALFText(const Text& /*text*/)
		: MixalException{"invalid ALF text"}
	{
	}
};

class DuplicateSymbolDefinitionError :
	public MixalException
{
public:
	DuplicateSymbolDefinitionError(const Symbol& /*symbol*/, const Word& /*value*/)
		: MixalException{"duplicate symbol definition"}
	{
	}
};

class UndefinedSymbolError :
	public MixalException
{
public:
	UndefinedSymbolError(const Symbol& /*symbol*/)
		: MixalException{"undefined symbol"}
	{
	}
};

class UnknownUnaryOperation :
	public MixalException
{
public:
	UnknownUnaryOperation(const UnaryOperation& /*op*/)
		: MixalException{"unknown unary operation"}
	{
	}
};

class UnknownBinaryOperation :
	public MixalException
{
public:
	UnknownBinaryOperation(const BinaryOperation& /*op*/)
		: MixalException{"unknown binary operation"}
	{
	}
};

class DivisionByZero :
	public MixalException
{
public:
	DivisionByZero()
		: MixalException{"division by zero"}
	{
	}
};

} // namespace mixal

