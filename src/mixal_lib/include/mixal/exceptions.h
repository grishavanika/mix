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
	InvalidALFText()
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

class InvalidLocalSymbolDefinition :
	public MixalException
{
public:
	InvalidLocalSymbolDefinition(const Symbol& /*symbol*/)
		: MixalException{"invalid local symbol definition"}
	{
	}
};

class InvalidLocalSymbolReference :
	public MixalException
{
public:
	InvalidLocalSymbolReference(const Symbol& /*symbol*/)
		: MixalException{"invalid local symbol reference"}
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

class InvalidWValueField :
	public MixalException
{
public:
	InvalidWValueField()
		: MixalException{"invalid W-value field"}
	{
	}
};

} // namespace mixal

