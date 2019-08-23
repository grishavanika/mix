#include <mix/command.h>

#include <iostream>
#include <iomanip>

using namespace mix;

namespace {

const WordField k_address_field{0, 2}; // +- AA
const std::size_t k_index_byte{3}; // I
const std::size_t k_field_byte{4}; // F
const std::size_t k_id_byte{5}; // C

} // namespace

Command::Command(const Word& word)
	: word_{word}
	, field_{WordField::FromByte(field_byte())}
	, address_{word.value(k_address_field)}
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
	change_address(address);
	word_.set_byte(k_index_byte, address_index);
	word_.set_byte(k_field_byte, field_.to_byte());
	word_.set_byte(k_id_byte, id);
}

std::size_t Command::id() const
{
	return word_.byte(k_id_byte).cast_to<std::size_t>();
}

const Byte& Command::field_byte() const
{
	return word_.byte(k_field_byte);
}

std::size_t Command::address_index() const
{
	return word_.byte(k_index_byte).cast_to<std::size_t>();
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

std::size_t Command::field() const
{
	return field_.to_byte().cast_to<std::size_t>();
}

void Command::change_address(WordValue address)
{
	address_ = address;
	word_.set_value(address_, k_address_field);
}

namespace mix {

std::ostream& operator<<(std::ostream& out, const Command& command)
{
	out << "|" << command.sign();
	out << "|" << ' ' << std::setw(4) << std::right << std::abs(command.address());
	out << "|" << std::setw(2) << std::right << command.address_index();
	out << "|" << std::setw(2) << std::right << command.field();
	out << "|" << std::setw(2) << std::right << command.id();
	out << "|";

	return out;
}

} // namespace mix

