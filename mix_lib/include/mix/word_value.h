#pragma once
#include <mix/general_enums.h>

namespace mix {

class WordValue
{
public:
	using Type = int;

	WordValue(Type value);
	WordValue(Sign sign, Type value);

	operator Type() const;

	Type value() const;
	Sign sign() const;

	WordValue reverse_sign() const;

private:
	Type value_;
	Sign sign_;
};

} // namespace mix


