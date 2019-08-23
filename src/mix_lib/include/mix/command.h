#pragma once
#include <mix/config.h>
#include <mix/word.h>
#include <mix/word_field.h>
#include <mix/registers.h>

#include <iosfwd>

namespace mix {

class MIX_LIB_EXPORT Command
{
public:
	explicit Command(const Word& word);
	
	explicit Command(
		std::size_t id,
		int address,
		std::size_t address_index,
		const WordField& field);

	explicit Command(
		std::size_t id,
		WordValue address,
		std::size_t address_index,
		const WordField& field);

	// Command: [0|1|2|3|4|5]
	/* [0]       +-*/Sign sign() const;
	/* [0,1,2] +-AA*/int address() const;
	/* [3]        I*/std::size_t address_index() const;
	/* [4]        F*/const WordField& word_field() const;
	/* [4]        F*/std::size_t field() const;
	/* [5]        C*/std::size_t id() const;

	// Shortcuts as named in specs
	int AA_with_sign() const { return address(); }
	std::size_t I() const    { return address_index(); }
	std::size_t F() const    { return field(); }
	std::size_t C() const    { return id(); }

	const Word& to_word() const;

	void change_address(WordValue address);

private:
	const Byte& field_byte() const;

private:
	Word word_;
	WordField field_;
	WordValue address_;
};

MIX_LIB_EXPORT
std::ostream& operator<<(std::ostream& out, const Command& command);

} // namespace mix


