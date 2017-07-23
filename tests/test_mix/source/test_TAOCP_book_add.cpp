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

class ADD_TAOCP_Book_Test : public ::testing::Test
{
private:
	void SetUp() override
	{
		Register ra;
		ra.set_byte(1, Byte{1});
		ra.set_byte(2, Byte{1});
		ra.set_byte(3, Byte{1});
		ra.set_byte(4, Byte{1});
		ra.set_byte(5, Byte{1});
		mix.set_ra(ra);
	}

protected:
	Computer mix;
};

} // namespace

TEST_F(ADD_TAOCP_Book_Test, Register_A_Bytes_Sum)
{
	mix.execute(MakeSTA(2000));
	mix.execute(MakeLDA(2000, WordField{5, 5}));

	mix.execute(MakeADD(2000, WordField{4, 4}));
	mix.execute(MakeADD(2000, WordField{3, 3}));
	mix.execute(MakeADD(2000, WordField{2, 2}));
	mix.execute(MakeADD(2000, WordField{1, 1}));

	ASSERT_EQ(5, mix.ra().value());
}
