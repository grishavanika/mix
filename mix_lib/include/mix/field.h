#pragma once
#include <mix/byte.h>

namespace mix {

class Field
{
public:
	explicit Field(std::size_t left, std::size_t right);

	Byte to_byte() const;
	
	static Field FromByte(const Byte& byte);

	bool has_only_sign() const;
	bool includes_sign() const;

	std::size_t left_byte_index() const;
	std::size_t right_byte_index() const;

	std::size_t bytes_count() const;

	Field shift_bytes_right() const;
	Field shift_bytes_left() const;

private:
	std::size_t left_;
	std::size_t right_;
};

} // namespace mix


