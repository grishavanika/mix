#pragma once
#include <ostream>

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

enum class DeviceType
{
	Unknown = -1,
	// https://en.wikipedia.org/wiki/Magnetic_tape
	MagneticTape = 7,
	// https://en.wikipedia.org/wiki/Drum_memory
	Drum = 15,
	// https://en.wikipedia.org/wiki/Punched_card
	PunchCard = 16,
	Perforator = 17,
	Printer = 18,
	Terminal = 19,
	// https://en.wikipedia.org/wiki/Punched_tape
	PunchedTape = 20,
};

using DeviceId = unsigned char;
using DeviceBlockId = int;

inline std::ostream& operator<<(std::ostream& o, Sign sign)
{
	o << ((sign == Sign::Positive) ? '+' : '-');
	return o;
}

} // namespace mix
