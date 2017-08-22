#include <mix/word.h>
#include <mix/exceptions.h>

#include <iomanip>

using namespace mix;

#if defined(__GNUC__)

// #TODO: work-around for MinGW linker error. Is it only for GCC on Windows ?
const unsigned int Byte::k_bits_count;
const Byte::NarrowType Byte::k_min_value;
const Byte::NarrowType Byte::k_max_value;

const std::size_t Word::k_bytes_count;
const std::size_t Word::k_bits_count;
const std::size_t Word::k_max_abs_value;
#endif

namespace {
int SignToInt(Sign sign)
{
	return (sign == Sign::Negative) ? -1 : 1;
}
} // namespace

/*static*/ WordField Word::MaxField()
{
	return WordField{0, k_bytes_count};
}

/*static*/ WordField Word::MaxFieldWithoutSign()
{
	return WordField{1, k_bytes_count};
}

/*static*/ bool Word::IsZero(const Word& value)
{
	return (value.abs_value() == 0);
}

/*static*/ bool Word::IsNegativeZero(const Word& value)
{
	return (value.sign() == mix::Sign::Negative) &&
		IsZero(value);
}

Word::Word()
	: bytes_()
	, sign_{Sign::Positive}
{
}

Word::Word(WordValue::Type value)
	: Word{WordValue{value}}
{
}

Word::Word(WordValue value, const WordField& field /*= MaxField()*/, bool owerwrite_sign /*= true*/)
	: Word{}
{
	set_value(value, field, owerwrite_sign);
}

Word::Word(BytesArray&& bytes, Sign sign /*= Sign::Positive*/)
	: bytes_(std::move(bytes))
	, sign_{sign}
{
}

Sign Word::sign() const
{
	return sign_;
}

int Word::sign_value() const
{
	return SignToInt(sign_);
}

void Word::set_sign(Sign sign)
{
	sign_ = sign;
}

const Byte& Word::byte(std::size_t index) const
{
	if ((index == 0) || (index > bytes_.size()))
	{
		throw InvalidWordByteIndex{index};
	}
	return bytes_[index - 1];
}

void Word::set_byte(std::size_t index, const Byte& byte)
{
	auto& self_byte = const_cast<Byte&>(static_cast<const Word&>(*this).byte(index));
	self_byte = byte;
}

void Word::set_value(WordValue value, const WordField& field, bool owerwrite_sign /*= true*/)
{
	const auto field_bytes = field.bytes_count();
	if (field_bytes > k_bytes_count)
	{
		throw TooBigWorldField{};
	}

	const auto field_bits = field_bytes * Byte::k_bits_count;
	if (field_bits >= (sizeof(int) * CHAR_BIT))
	{
		throw TooBigWorldField{};
	}

	const std::size_t abs_value = static_cast<std::size_t>(std::abs(value));

	const std::size_t max_int_for_field = (1 << field_bits) - 1;

	if (!field.has_only_sign() &&
		(max_int_for_field < abs_value))
	{
		throw TooBigWorldValue{};
	}

	set_value(abs_value, value.sign(), field, field.includes_sign() || owerwrite_sign);
}

void Word::set_value(std::size_t value, Sign sign, const WordField& field, bool owerwrite_sign)
{
	if (owerwrite_sign)
	{
		set_sign(sign);
	}

	if (field.has_only_sign())
	{
		return;
	}

	std::size_t start = field.left_byte_index();
	std::size_t end = field.right_byte_index();

	for ( ; end >= start; --end)
	{
		// #TODO: maybe provide Byte::set_with_overdlow() function ?
		const auto byte_value = value & Byte::k_max_value;
		set_byte(end, byte_value);
		
		value >>= Byte::k_bits_count;
	}
}

void Word::set_value(WordValue value)
{
	set_value(value, MaxField(), true/*do not ignore sign*/);
}

void Word::set_zero_abs_value()
{
	set_value(0, MaxFieldWithoutSign(), false/*ignore sign*/);
}

WordValue Word::value(const WordField& field, bool take_sign /*= false*/) const
{
	auto sign = sign_;
	if (!take_sign && !field.includes_sign())
	{
		sign = Sign::Positive;
	}

	if (field.has_only_sign())
	{
		return WordValue{sign, 0};
	}

	std::size_t value = 0;
	std::size_t start = field.left_byte_index();
	std::size_t end = field.right_byte_index();

	for (std::size_t i = 0; end >= start; --end, ++i)
	{
		std::size_t mask = byte(end).cast_to<std::size_t>();
		mask <<= (i * Byte::k_bits_count);
		value |= mask;
	}

	return WordValue{sign, static_cast<int>(value) * SignToInt(sign)};
}

WordValue Word::value() const
{
	return value(MaxField());
}

std::size_t Word::abs_value() const
{
	return static_cast<std::size_t>(value(MaxFieldWithoutSign()));
}

const Word::BytesArray& Word::bytes() const
{
	return bytes_;
}

namespace mix {

bool operator==(const Word& lhs, const Word& rhs)
{
	return (lhs.sign_ == rhs.sign_) &&
		(lhs.bytes_ == rhs.bytes_);
}

std::ostream& operator<<(std::ostream& o, const Word& w)
{
	o << '|' << w.sign() << '|';
	for (const auto& b : w.bytes())
	{
		o << std::setw(2) << b << '|';
	}
	return o;
}

} // namespace mix


