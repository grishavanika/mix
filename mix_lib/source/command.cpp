#include <mix/command.h>

using namespace mix;

Command::Command(const Word& word)
	: word_{word}
	, field_{WordField::FromByte(field_byte())}
	, address_{word.value(WordField{0, 2})}
{
}

Command::Command(
	std::size_t id,
	int address,
	std::size_t address_index,
	const WordField& field)
		: Command{id, WordValue{address}, address_index, field}
{
}

Command::Command(
	std::size_t id,
	WordValue address,
	std::size_t address_index,
	const WordField& field)
		: word_{}
		, field_{field}
		, address_{address}
{
	word_.set_value(address_, WordField{0, 2});
	word_.set_byte(3, Byte{address_index});
	word_.set_byte(4, field_.to_byte());
	word_.set_byte(5, Byte{id});
}

std::size_t Command::id() const
{
	return word_.byte(5).cast_to<std::size_t>();
}

const Byte& Command::field_byte() const
{
	return word_.byte(4);
}

std::size_t Command::address_index() const
{
	return word_.byte(3).cast_to<std::size_t>();
}

const WordField& Command::word_field() const
{
	return field_;
}

const Word& Command::to_word() const
{
	return word_;
}

int Command::address() const
{
	return address_.value();
}

Sign Command::sign() const
{
	return word_.sign();
}
