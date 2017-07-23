#include <mix/commands_factory.h>

namespace mix {

namespace {
Command make_command(std::size_t id, int address, const WordField& field, std::size_t index_register)
{
	return Command{Byte{id}, AddressRegister{address}, Byte{index_register}, field};
}
} // namespace

Command make_lda(int address, const WordField& field, std::size_t index_register)
{
	return make_command(8, address, field, index_register);
}

Command make_lda(int address, const WordField& field)
{
	return make_lda(address, field, 0);
}

Command make_lda(int address, std::size_t index_register)
{
	return make_lda(address, Word::MaxField(), index_register);
}

Command make_lda(int address)
{
	return make_lda(address, 0);
}

Command make_sta(int address, const WordField& field, std::size_t index_register)
{
	return make_command(24, address, field, index_register);
}

Command make_sta(int address, const WordField& field)
{
	return make_sta(address, field, 0);
}

Command make_sta(int address, std::size_t index_register)
{
	return make_sta(address, Word::MaxField(), index_register);
}

Command make_sta(int address)
{
	return make_sta(address, 0);
}

} // namespace mix
