#pragma once
#include <mix/general_enums.h>

#include <cstddef>

namespace mix {

class WordValue
{
public:
	// #TODO: should be std::int32_t.
	// Also, `abs_value()` should return std::uint32_t
	using Type = int;

	WordValue(Type value);
	WordValue(Sign sign, Type value);

	operator Type() const;

	Type value() const;
	std::size_t abs_value() const;
	Sign sign() const;

	WordValue reverse_sign() const;

private:
	Type value_;
	Sign sign_;
};

} // namespace mix


