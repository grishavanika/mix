#include <mix/word_field.h>
#include <mix/word.h>

#include <ostream>

#include <cassert>

using namespace mix;

WordField::WordField(std::size_t left, std::size_t right)
	: left_{left}
	, right_{right}
{
	// #TODO: review code and validate that there are no
	// statements that expect:
	// 1. `right` to be less then equal to `Word::k_bytes_count`
	// 2. `left` to be less then equal to `right`
}

Byte WordField::to_byte() const
{
	const auto value = 8 * left_ + right_;
	return value;
}

WordField WordField::FromByte(const Byte& byte)
{
	const auto value = byte.cast_to<std::size_t>();
	return WordField{value / 8, value % 8};
}

bool WordField::has_only_sign() const
{
	return (left_ == 0) && (right_ == 0);
}

bool WordField::includes_sign() const
{
	return (left_ == 0);
}

std::size_t WordField::left_byte_index() const
{
	const bool need_shift = (left_ == 0) && (left_ < right_);
	return left_ + (need_shift ? 1 : 0);
}

std::size_t WordField::right_byte_index() const
{
	return right_;
}

std::size_t WordField::left() const
{
	return left_;
}

std::size_t WordField::right() const
{
	return right_;
}


std::size_t WordField::bytes_count() const
{
	assert(right_ >= left_);
	auto length = right_ - left_;
	if (!includes_sign())
	{
		++length;
	}
	return length;
}

WordField WordField::shift_bytes_right() const
{
	if (has_only_sign())
	{
		return *this;
	}

	assert(right_byte_index() <= Word::k_bytes_count);
	assert(right_ >= left_);;

	const auto shift_right = Word::k_bytes_count - right_byte_index();
	return WordField{left_byte_index() + shift_right, Word::k_bytes_count};
}

namespace mix {

bool operator==(const WordField& lhs, const WordField& rhs)
{
	return (lhs.left_ == rhs.left_) &&
		(lhs.right_ == rhs.right_);
}

std::ostream& operator<<(std::ostream& o, const WordField& wf)
{
	o << '(' << wf.left_byte_index()
		<< ':' << wf.right_byte_index()
		<< ')';
	return o;
}

} // namespace mix
