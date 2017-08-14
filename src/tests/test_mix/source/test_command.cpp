#include "precompiled.h"

using namespace mix;

namespace {
Word PackToCommandWord(std::size_t id, int address, std::size_t index_register, const WordField& field)
{
	Word w;
	w.set_byte(5, id);
	w.set_byte(4, field.to_byte());
	w.set_byte(3, index_register);
	w.set_value(address, WordField{0, 2});
	return w;
}
} // namespace

TEST(Command, Word_Constructor_Upacks)
{
	const auto word = PackToCommandWord(1, -200, 4, WordField{1, 3});
	const Command command{word};
	ASSERT_EQ(1u, command.id());
	ASSERT_EQ(-200, command.address());
	ASSERT_EQ(WordField(1, 3), command.word_field());
	ASSERT_EQ(4u, command.address_index());
	ASSERT_EQ(word, command.to_word());
}

TEST(Command, Parts_Constructor_Packs)
{
	const auto word = PackToCommandWord(1, -200, 4, WordField{1, 3});
	const Command command{1, -200, 4, WordField{1, 3}};
	
	ASSERT_EQ(1u, command.id());
	ASSERT_EQ(-200, command.address());
	ASSERT_EQ(WordField(1, 3), command.word_field());
	ASSERT_EQ(4u, command.address_index());
	ASSERT_EQ(word, command.to_word());
}

TEST(Command, Command_With_Negative_Zero_Address_Can_Be_Constructed)
{
	const Command command{1, WordValue{Sign::Negative, 0}, 3, WordField{1, 3}};
	ASSERT_EQ(0, command.address());
	ASSERT_EQ(Sign::Negative, command.sign());
}
