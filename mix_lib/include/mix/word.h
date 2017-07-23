#pragma once
#include <mix/byte.h>
#include <mix/general_enums.h>
#include <mix/word_field.h>
#include <mix/word_value.h>

#include <array>

#include <climits>

namespace mix {

class Word
{
public:
	static constexpr std::size_t k_bytes_count = 5;
	static constexpr std::size_t k_bits_count = (k_bytes_count * Byte::k_bits_count);
	static constexpr std::size_t k_max_abs_value = (1 << k_bits_count) - 1;

	explicit Word();

	Sign sign() const;
	int sign_value() const;
	void set_sign(Sign sign);

	// Starting from 1:
	// [ +- | 1 | 2 | 3 | 4 | 5 ]
	const Byte& byte(std::size_t index) const;
	void set_byte(std::size_t index, const Byte& byte);

	static_assert(static_cast<std::size_t>(INT_MAX) >= k_max_abs_value,
		"Selected bytes count is too big to implement value()/set_value() functions. "
		"Either decrease bytes count or change int to more capable signed type");

	void set_value(WordValue value, const WordField& field, bool owerwrite_sign = true);
	void set_value(WordValue value);
	WordValue value(const WordField& field, bool take_sign = false) const;
	WordValue value() const;

	static WordField MaxField();

private:
	void set_value(std::size_t value, Sign sign, const WordField& field, bool owerwrite_sign);

	friend bool operator==(const Word& lhs, const Word& rhs);

private:
	std::array<Byte, k_bytes_count> bytes_;
	Sign sign_;
};

bool operator==(const Word& lhs, const Word& rhs);

} // namespace mix


