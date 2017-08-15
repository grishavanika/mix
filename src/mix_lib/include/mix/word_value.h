#pragma once
#include <mix/config.h>
#include <mix/general_types.h>

#include <cstddef>

namespace mix {

class MIX_LIB_EXPORT WordValue
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


