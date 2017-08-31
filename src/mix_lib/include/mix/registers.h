#pragma once
#include <mix/config.h>
#include <mix/word.h>

#include <cstdint>

namespace mix {

class MIX_LIB_EXPORT Register : public Word
{
public:
	using Word::Word;
};

// #TODO: use composition over inheritance
class MIX_LIB_EXPORT IndexRegister : public Register
{
public:
	explicit IndexRegister();
	explicit IndexRegister(int two_bytes_with_sign);
	explicit IndexRegister(const Byte& b4, const Byte& b5);
	explicit IndexRegister(Sign sign, const Byte& b4, const Byte& b5);
	explicit IndexRegister(const Register& r);

	void set_byte(std::size_t index, const Byte& byte);
	
	void set_value(int two_bytes_with_sign);
	WordValue value() const;

	void zero_unspecified_bytes();

private:
	bool is_undefined_index(std::size_t index) const;
};

class MIX_LIB_EXPORT AddressRegister : public IndexRegister
{
public:
	using IndexRegister::IndexRegister;

	// #TODO: `sing() == Sign::Positive` should be true always
};

struct RAX
{
	Register ra;
	Register rx;

	static constexpr std::size_t k_bytes_count = 2 * Register::k_bytes_count;
	using Type = std::uint64_t;
};

static_assert((sizeof(RAX::Type) * CHAR_BIT) >= (2 * Register::k_bits_count),
	"Native `uint64_t` can't hold the result of RAX command");

} // namespace mix

