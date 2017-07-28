#pragma once
#include <mix/word.h>
#include <mix/word_field.h>
#include <mix/registers.h>

namespace mix {

class Command
{
public:
	explicit Command(const Word& word);
	
	// #TODO: `address` should be `WordValue` since it can be
	// -0 or +0 and we need to distinguish between these values
	explicit Command(
		std::size_t id,
		int address,
		std::size_t address_index,
		const WordField& field);

	std::size_t id() const;
	const WordField& word_field() const;
	int address() const;
	std::size_t address_index() const;

	const Word& to_word() const;

private:
	const Byte& field_byte() const;

private:
	Word word_;
	WordField field_;
	AddressRegister address_;
};

} // namespace mix


