#include <mix/computer.h>
#include <mix/command.h>

#include <gtest/gtest.h>

using namespace mix;

namespace {

Word MakeTestWord(int address, int b3, int b4, int b5)
{
	Word data;
	data.set_value(address, WordField{0, 2});
	data.set_byte(3, Byte{b3});
	data.set_byte(4, Byte{b4});
	data.set_byte(5, Byte{b5});
	return data;
}

Command MakeLDA(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{8, address, index_register, field};
}

class LDA_TAOCP_Book_Test : public ::testing::Test
{
private:
	void SetUp() override
	{
		source_address = 2000;
		source_cell = MakeTestWord(-80, 3, 5, 4);
		mix.set_memory(static_cast<int>(source_address), source_cell);

		Register ra;
		ra.set_value(Word::k_max_abs_value);
		mix.set_ra(ra);
	}

protected:
	Computer mix;
	Word source_cell;
	int source_address;
};

} // namespace

// LDA 2000
TEST_F(LDA_TAOCP_Book_Test, Default_LDA_Gets_All_Word)
{
	mix.execute(MakeLDA(source_address));
	ASSERT_EQ(source_cell, mix.ra());
}

// LDA 2000(1:5)
TEST_F(LDA_TAOCP_Book_Test, LDA_WithoutSign_Gets_Word_WithoutSign)
{
	mix.execute(MakeLDA(source_address, WordField{1, 5}));
	
	ASSERT_EQ(Sign::Positive, mix.ra().sign());
	
	ASSERT_EQ(mix.ra().byte(1), source_cell.byte(1));
	ASSERT_EQ(mix.ra().byte(2), source_cell.byte(2));
	ASSERT_EQ(mix.ra().byte(3), source_cell.byte(3));
	ASSERT_EQ(mix.ra().byte(4), source_cell.byte(4));
	ASSERT_EQ(mix.ra().byte(5), source_cell.byte(5));
}

// LDA 2000(3:5)
TEST_F(LDA_TAOCP_Book_Test, LDA_With_CustomFild_MapsToTheSameField_If_CantBe_ShiftedRight)
{
	mix.execute(MakeLDA(source_address, WordField{3, 5}));

	ASSERT_EQ(Sign::Positive, mix.ra().sign());

	ASSERT_EQ(mix.ra().byte(1), Byte{0});
	ASSERT_EQ(mix.ra().byte(2), Byte{0});
	ASSERT_EQ(mix.ra().byte(3), source_cell.byte(3));
	ASSERT_EQ(mix.ra().byte(4), source_cell.byte(4));
	ASSERT_EQ(mix.ra().byte(5), source_cell.byte(5));
}

// LDA 2000(0:3)
TEST_F(LDA_TAOCP_Book_Test, LDA_With_CustomFild_WithSign_ShiftedRight)
{
	mix.execute(MakeLDA(source_address, WordField{0, 3}));

	ASSERT_EQ(Sign::Negative, mix.ra().sign());

	ASSERT_EQ(mix.ra().byte(1), Byte{0});
	ASSERT_EQ(mix.ra().byte(2), Byte{0});
	ASSERT_EQ(mix.ra().byte(3), source_cell.byte(1));
	ASSERT_EQ(mix.ra().byte(4), source_cell.byte(2));
	ASSERT_EQ(mix.ra().byte(5), source_cell.byte(3));
}

// LDA 2000(4:4)
TEST_F(LDA_TAOCP_Book_Test, LDA_With_CustomFild_ShiftedRight)
{
	mix.execute(MakeLDA(source_address, WordField{4, 4}));

	ASSERT_EQ(Sign::Positive, mix.ra().sign());

	ASSERT_EQ(mix.ra().byte(1), Byte{0});
	ASSERT_EQ(mix.ra().byte(2), Byte{0});
	ASSERT_EQ(mix.ra().byte(3), Byte{0});
	ASSERT_EQ(mix.ra().byte(4), Byte{0});
	ASSERT_EQ(mix.ra().byte(5), source_cell.byte(4));
}

// LDA 2000(0:0)
TEST_F(LDA_TAOCP_Book_Test, LDA_With_OnlySign_Gets_Only_Sign)
{
	mix.execute(MakeLDA(source_address, WordField{0, 0}));

	ASSERT_EQ(Sign::Negative, mix.ra().sign());

	ASSERT_EQ(mix.ra().byte(1), Byte{0});
	ASSERT_EQ(mix.ra().byte(2), Byte{0});
	ASSERT_EQ(mix.ra().byte(3), Byte{0});
	ASSERT_EQ(mix.ra().byte(4), Byte{0});
	ASSERT_EQ(mix.ra().byte(5), Byte{0});
}

// LDA 2000(1:1)
TEST_F(LDA_TAOCP_Book_Test, LDA_With_FirstByte_Is_ShiftedRight)
{
	mix.execute(MakeLDA(source_address, WordField{1, 1}));

	ASSERT_EQ(Sign::Positive, mix.ra().sign());

	ASSERT_EQ(mix.ra().byte(1), Byte{0});
	ASSERT_EQ(mix.ra().byte(2), Byte{0});
	ASSERT_EQ(mix.ra().byte(3), Byte{0});
	ASSERT_EQ(mix.ra().byte(4), Byte{0});
	ASSERT_EQ(mix.ra().byte(5), source_cell.byte(1));
}
