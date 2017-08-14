#pragma once
#include <mix/word.h>
#include <mix/word_field.h>
#include <mix/registers.h>

namespace mix {

class Command
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

private:
	const Byte& field_byte() const;

private:
	Word word_;
	WordField field_;
	WordValue address_;
};

} // namespace mix


