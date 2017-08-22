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

	std::size_t id() const;
	const WordField& word_field() const;
	std::size_t field() const;
	int address() const;
	std::size_t address_index() const;
	Sign sign() const;

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


