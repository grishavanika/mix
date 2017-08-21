#include <mix/word_value.h>

#include <cmath>

using namespace mix;


WordValue::WordValue(Type value)
	: WordValue{value >= 0 ? Sign::Positive : Sign::Negative, value}
{
}

WordValue::WordValue(Sign sign, Type value)
	: value_{value}
	, sign_{sign}
{
}

WordValue::operator Type() const
{
	return value();
}

WordValue::Type WordValue::value() const
{
	return value_;
}

std::size_t WordValue::abs_value() const
{
	return static_cast<std::size_t>(std::abs(value_));
}

Sign WordValue::sign() const
{
	return sign_;
}

WordValue WordValue::reverse_sign() const
{
	const auto sign = ((sign_ == Sign::Negative) ? Sign::Positive : Sign::Negative);
	const auto value = value_ * -1;
	return WordValue{sign, value};
}

namespace mix {

bool operator==(const WordValue& lhs, const WordValue& rhs)
{
	return (lhs.value() == rhs.value()) &&
		(lhs.sign() == rhs.sign());
}

bool operator==(const WordValue& lhs, WordValue::Type rhs)
{
	return (lhs.value() == rhs);
}

bool operator==(WordValue::Type lhs, const WordValue& rhs)
{
	return (lhs == rhs.value());
}

} // namespace mix
