#pragma once
#include <mix/byte.h>

namespace mix {

class WordField
{
public:
	explicit WordField(std::size_t left, std::size_t right);
	static WordField FromByte(const Byte& byte);

	Byte to_byte() const;

	// For case when (0:0) was specified
	bool has_only_sign() const;
	// For (0:...5) case
	bool includes_sign() const;

	std::size_t left_byte_index() const;
	std::size_t right_byte_index() const;

	std::size_t bytes_count() const;

	WordField shift_bytes_right() const;

private:
	friend bool operator==(const WordField& lhs, const WordField& rhs);

private:
	std::size_t left_;
	std::size_t right_;
};

bool operator==(const WordField& lhs, const WordField& rhs);

} // namespace mix


