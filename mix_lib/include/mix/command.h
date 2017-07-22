#pragma once
#include <mix/word.h>
#include <mix/field.h>
#include <mix/registers.h>

namespace mix {

class Command
{
public:
	explicit Command(const Word& word);
	
	explicit Command(
		const Byte& id,
		const AddressRegister& address,
		const Byte& address_index,
		const Field& field);

	std::size_t id() const;
	const Field& field() const;
	int address() const;
	std::size_t address_index() const;

	const Word& to_word() const;

private:
	const Byte& field_byte() const;

private:
	Word word_;
	Field field_;
	AddressRegister address_;
};

} // namespace mix


