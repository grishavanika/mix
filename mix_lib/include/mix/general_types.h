#pragma once

namespace mix {

enum class Sign
{
	Positive	= 1,
	Negative	= -1
};

enum class ComparisonIndicator
{
	Equal		= 0,
	Less		= -1,
	Greater		= 1
};

enum class OverflowFlag
{
	NoOverdlow	= 0,
	Overflow	= 1
};

using DeviceId = unsigned char;

} // namespace mix
