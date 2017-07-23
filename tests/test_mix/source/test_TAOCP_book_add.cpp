#include <mix/computer.h>
#include <mix/command.h>

#include <gtest/gtest.h>

using namespace mix;

namespace {

Command MakeSTA(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{24, address, index_register, field};
}

Command MakeLDA(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{8, address, index_register, field};
}

Command MakeADD(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{1, address, index_register, field};
}
} // namespace

TEST(ADD_TAOCP_Book_Test, Register_A_Bytes_Sum)
{
	const auto k_byte{Byte::Max()};

	Computer mix;

	{
		Register ra;
		ra.set_byte(1, k_byte);
		ra.set_byte(2, k_byte);
		ra.set_byte(3, k_byte);
		ra.set_byte(4, k_byte);
		ra.set_byte(5, k_byte);
		mix.set_ra(ra);
	}

	mix.execute(MakeSTA(2000));
	mix.execute(MakeLDA(2000, WordField{5, 5}));

	mix.execute(MakeADD(2000, WordField{4, 4}));
	mix.execute(MakeADD(2000, WordField{3, 3}));
	mix.execute(MakeADD(2000, WordField{2, 2}));
	mix.execute(MakeADD(2000, WordField{1, 1}));

	const auto result = mix.ra().value();
	const auto expected_result = k_byte.value() * 5;
	ASSERT_EQ(expected_result, result);
}

TEST(ADD_TAOCP_Book_Test, Simple_Addition)
{
	Computer mix;

	{
		Register ra;
		ra.set_value(1234, WordField{0, 2});
		ra.set_byte(3, Byte{1});
		ra.set_value(150, WordField{4, 5});
		mix.set_ra(ra);
	}

	{
		Word w;
		w.set_value(100, WordField{0, 2});
		w.set_byte(3, Byte{5});
		w.set_value(50, WordField{4, 5});
		mix.set_memory(1000, w);
	}

	mix.execute(MakeADD(1000));

	{
		ASSERT_EQ(1334, mix.ra().value(WordField{0, 2}));
		ASSERT_EQ(6, mix.ra().value(WordField{3, 3}));
		ASSERT_EQ(200, mix.ra().value(WordField{4, 5}));
	}
}

