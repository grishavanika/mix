#include <mix/field.h>
#include <mix/word.h>

using namespace mix;

Field::Field(std::size_t left, std::size_t right)
	: left_{left}
	, right_{right}
{
	if (left > right)
	{
		throw std::logic_error{"Field can be used only for adjacent indexes"};
	}

	if (right > Word::k_bytes_count)
	{
		throw std::logic_error{"Too big Field was specified for Word"};
	}
}

Byte Field::to_byte() const
{
	const auto value = 8 * left_ + right_;
	return Byte{value};
}

Field Field::FromByte(const Byte& byte)
{
	const auto value = byte.cast_to<std::size_t>();
	return Field{value / 8, value % 8};
}

bool Field::has_only_sign() const
{
	return (left_ == 0) && (right_ == 0);
}

bool Field::includes_sign() const
{
	return (left_ == 0) && (right_ >= 0);
}

std::size_t Field::left_byte_index() const
{
	const bool need_shift = (left_ == 0) && (left_ < right_);
	return left_ + (need_shift ? 1 : 0);
}

std::size_t Field::right_byte_index() const
{
	return right_;
}

std::size_t Field::bytes_count() const
{
	auto length = right_ - left_;
	if (!includes_sign())
	{
		++length;
	}
	return length;
}

Field Field::shift_bytes_right() const
{
	if (has_only_sign())
	{
		return *this;
	}

	const auto shift_right = Word::k_bytes_count - right_byte_index();
	return Field{left_byte_index() + shift_right, Word::k_bytes_count};
}

