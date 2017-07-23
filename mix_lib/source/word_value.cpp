#include <mix/word_value.h>

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

Sign WordValue::sign() const
{
	return sign_;
}

