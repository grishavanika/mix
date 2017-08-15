#pragma once
#include <mix/config.h>
#include <mix/word.h>

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

} // namespace mix

