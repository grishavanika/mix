#pragma once
#include <mix/byte.h>
#include <mix/registers.h>

// Used as easy-to-use implementation of shift commands
#include <valarray>
#include <utility>

#include <cassert>

namespace mix {
namespace internal {

// #TODO: think how this can be removed or write better.
// Currently it's used as easy-to-implement shift-left/right/cyclic
// operations with one or two registers

inline std::valarray<Byte> ToBytes(const Register& r)
{
	const auto& bytes = r.bytes();
	return {bytes.data(), bytes.size()};
}

inline std::valarray<Byte> ToBytes(const Register& r1, const Register& r2)
{
	std::valarray<Byte> result{2 * Word::k_bytes_count};

	std::copy(begin(r1.bytes()), end(r1.bytes()), begin(result));
	std::copy(begin(r2.bytes()), end(r2.bytes()), begin(result) + Word::k_bytes_count);
	return result;
}

inline Register ToRegister(Sign sign, const std::valarray<Byte>& bytes)
{
	assert(bytes.size() == 5);
	Register r;
	r.set_sign(sign);
	for (std::size_t i = 1; i <= 5; ++i)
	{
		r.set_byte(i, bytes[i - 1]);
	}
	return r;
}

inline std::pair<Register, Register> ToRegisters(Sign sign1, Sign sign2, const std::valarray<Byte>& bytes)
{
	assert(bytes.size() == (2 * 5));
	Register r1;
	r1.set_sign(sign1);
	Register r2;
	r2.set_sign(sign2);
	for (std::size_t i = 1; i <= 5; ++i)
	{
		r1.set_byte(i, bytes[i - 1]);
		r2.set_byte(i + 5, bytes[i + 5 - 1]);
	}
	return std::make_pair(std::move(r1), std::move(r2));
}

} // namespace internal
} // namespace mix
