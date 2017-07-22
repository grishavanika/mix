#include <mix/word.h>

using namespace mix;

Field Word::MaxField()
{
	return Field{0, k_bytes_count};
}

Word::Word()
	: bytes_{}
	, sign_{Sign::Positive}
{
}

Sign Word::sign() const
{
	return sign_;
}

void Word::set_sign(Sign sign)
{
	sign_ = sign;
}

const Byte& Word::byte(std::size_t index) const
{
	if ((index == 0) || (index > bytes_.size()))
	{
		throw std::out_of_range{"Invalid byte's index for Word"};
	}
	return bytes_[index - 1];
}

void Word::set_byte(std::size_t index, const Byte& byte)
{
	auto& self_byte = const_cast<Byte&>(static_cast<const Word&>(*this).byte(index));
	self_byte = byte;
}

void Word::set_value(int value, const Field& field, bool owerwrite_sign /*= true*/)
{
	const auto sign = (value >= 0) ? Sign::Positive : Sign::Negative;

	const auto field_bytes = field.bytes_count();
	if (field_bytes > k_bytes_count)
	{
		throw std::logic_error{"Too big `Field` was specified. "
			"Impossible to set to `Word`"};
	}

	const auto field_bits = field_bytes * Byte::k_bits_count;
	if (field_bits >= (sizeof(int) * CHAR_BIT))
	{
		throw std::logic_error{"Too big `Field` was specified. "
			"Impossible to set from `int`"};
	}

	const std::size_t abs_value = static_cast<std::size_t>(std::abs(value));

	const std::size_t max_int_for_field = (1 << field_bits) - 1;

	if (!field.has_only_sign() &&
		(max_int_for_field < abs_value))
	{
		throw std::logic_error{"Too big int was specified. "
			"Impossible to set to given `Field`"};
	}

	set_value(abs_value, sign, field, field.includes_sign() || owerwrite_sign);
}

void Word::set_value(std::size_t value, Sign sign, const Field& field, bool owerwrite_sign)
{
	if (owerwrite_sign)
	{
		set_sign(sign);
	}

	std::size_t start_index = field.left() + (field.includes_sign() ? 1 : 0);
	std::size_t end_index = field.right();

	for ( ; start_index <= end_index; ++start_index)
	{
		// #TODO: maybe provide Byte::set_with_overdlow() function ?
		const auto byte_value = value & Byte::k_max_value;
		set_byte(start_index, Byte{byte_value});
		
		value >>= Byte::k_bits_count;
	}
}

void Word::set_value(int value)
{
	set_value(value, MaxField(), true/*do not ignore sign*/);
}

int Word::value(const Field& field, bool ignore_sign /*= false*/) const
{
	std::size_t start_index = field.left() + (field.includes_sign() ? 1 : 0);
	std::size_t end_index = field.right();

	std::size_t value = 0;
	for (std::size_t i = 0; start_index <= end_index; ++start_index, ++i)
	{
		std::size_t mask = byte(start_index).cast_to<std::size_t>();
		mask <<= (i * Byte::k_bits_count);
		value |= mask;
	}

	const int sign_value = (!ignore_sign && (sign_ == Sign::Negative) ? -1 : 1);
	return static_cast<int>(value) * sign_value;
}

int Word::value(bool ignore_sign /*= false*/) const
{
	return value(MaxField(), ignore_sign);
}

namespace mix {

bool operator==(const Word& lhs, const Word& rhs)
{
	return (lhs.sign_ == rhs.sign_) &&
		(lhs.bytes_ == rhs.bytes_);
}

} // namespace mix


