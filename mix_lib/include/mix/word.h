#pragma once
#include <mix/byte.h>
#include <mix/general_enums.h>

#include <array>

namespace mix {

class Word
{
public:
	static constexpr std::size_t k_bytes_count = 5;

	explicit Word();

	Sign sign() const;
	void set_sign(Sign sign);

	// Starting from 1:
	// [ +- | 1 | 2 | 3 | 4 | 5 ]
	const Byte& byte(std::size_t index) const;
	void set_byte(std::size_t index, const Byte& byte);

private:
	std::array<Byte, k_bytes_count> bytes_;
	Sign sign_;
};

} // namespace mix


