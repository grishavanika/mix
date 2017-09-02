#pragma once
#include <ostream>

struct OptionalOStream
{
	std::ostream& stream;
	bool enabled;

	OptionalOStream(std::ostream& out, bool enable)
		: stream{out}
		, enabled{enable}
	{
	}
};

template<typename T>
OptionalOStream& operator<<(OptionalOStream& out, const T& value)
{
	if (out.enabled)
	{
		out.stream << value;
	}
	return out;
}

