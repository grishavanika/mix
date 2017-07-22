#include <mix/commands_factory.h>

namespace mix {

Command make_lda(int address, const Field& field, std::size_t index_register)
{
	return Command{Byte{8}, AddressRegister{address}, Byte{index_register}, field};
}

Command make_lda(int address, const Field& field)
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

} // namespace mix
