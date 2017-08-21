#pragma once
#include <stdexcept>

namespace mix {

class MixException :
	public std::logic_error
{
public:
	using Base = std::logic_error;
	using Base::Base;
};

class ByteOverflow :
	public MixException
{
public:
	ByteOverflow()
		: MixException{"byte overflow"}
	{
	}
};

class InvalidWordByteIndex :
	public MixException
{
public:
	InvalidWordByteIndex(std::size_t /*index*/)
		: MixException{"invalid word byte index"}
	{
	}
};

class TooBigWorldField :
	public MixException
{
public:
	TooBigWorldField()
		: MixException{"too big world field"}
	{
	}
};

class TooBigWorldValue :
	public MixException
{
public:
	TooBigWorldValue()
		: MixException{"too big world value"}
	{
	}
};

class InvalidMemoryAddressIndex :
	public MixException
{
public:
	InvalidMemoryAddressIndex(int /*index*/)
		: MixException{"invalid memory index"}
	{
	}
};

class InvalidIndexRegister :
	public MixException
{
public:
	InvalidIndexRegister(std::size_t /*index*/)
		: MixException{"invalid index register"}
	{
	}
};

class UnknownCommandField :
	public MixException
{
public:
	UnknownCommandField()
		: MixException{"unknown command field"}
	{
	}
};

} // namespace mix

