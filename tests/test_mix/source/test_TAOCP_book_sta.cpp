#include "precompiled.h"

using namespace mix;

namespace {

Command MakeSTA(int address, const WordField& field = Word::MaxField(), std::size_t index_register = 0)
{
	return Command{24, address, index_register, field};
}

class STA_TAOCP_Book_Test : public ::testing::Test
{
private:
	void SetUp() override
	{
		dest_address = 2000;
		dest_cell.set_sign(Sign::Negative);
		dest_cell.set_byte(1, 1);
		dest_cell.set_byte(2, 2);
		dest_cell.set_byte(3, 3);
		dest_cell.set_byte(4, 4);
		dest_cell.set_byte(5, 5);

		mix.set_memory(dest_address, dest_cell);

		Register ra;
		ra.set_sign(Sign::Positive);
		ra.set_byte(1, 6);
		ra.set_byte(2, 7);
		ra.set_byte(3, 8);
		ra.set_byte(4, 9);
		ra.set_byte(5, 10);

		mix.set_ra(ra);
	}

protected:
	const Word& dest_word()
	{
		return mix.memory(static_cast<std::size_t>(dest_address));
	}

protected:
	Computer mix;
	Word dest_cell;
	int dest_address;
};
} // namespace

// STA 2000
TEST_F(STA_TAOCP_Book_Test, Default_STA_Stores_All_Word)
{
	mix.execute(MakeSTA(dest_address));
	ASSERT_EQ(mix.ra(), dest_word());
}

// STA 2000(1:5)
TEST_F(STA_TAOCP_Book_Test, STA_With_All_BytesField_Gets_All_Except_Sign)
{
	mix.execute(MakeSTA(dest_address, WordField{1, 5}));

	ASSERT_EQ(dest_word().sign(), Sign::Negative);
	ASSERT_EQ(dest_word().byte(1), 6);
	ASSERT_EQ(dest_word().byte(2), 7);
	ASSERT_EQ(dest_word().byte(3), 8);
	ASSERT_EQ(dest_word().byte(4), 9);
	ASSERT_EQ(dest_word().byte(5), 10);
}

// STA 2000(5:5)
TEST_F(STA_TAOCP_Book_Test, STA_With_RightMost_Field_Sets_Byte_Without_Shift)
{
	mix.execute(MakeSTA(dest_address, WordField{5, 5}));

	ASSERT_EQ(dest_word().sign(), Sign::Negative);
	ASSERT_EQ(dest_word().byte(1), 1);
	ASSERT_EQ(dest_word().byte(2), 2);
	ASSERT_EQ(dest_word().byte(3), 3);
	ASSERT_EQ(dest_word().byte(4), 4);
	ASSERT_EQ(dest_word().byte(5), 10);
}

// STA 2000(2:2)
TEST_F(STA_TAOCP_Book_Test, STA_With_One_ByteField_Length_Gets_Value_From_5_RA_Byte)
{
	mix.execute(MakeSTA(dest_address, WordField{2, 2}));
	ASSERT_EQ(mix.ra().byte(5), dest_word().byte(2));

	ASSERT_EQ(dest_word().sign(), Sign::Negative);
	ASSERT_EQ(dest_word().byte(1), 1);
	ASSERT_EQ(dest_word().byte(2), 10);
	ASSERT_EQ(dest_word().byte(3), 3);
	ASSERT_EQ(dest_word().byte(4), 4);
	ASSERT_EQ(dest_word().byte(5), 5);
}

// STA 2000(2:3)
TEST_F(STA_TAOCP_Book_Test, STA_With_Two_ByteField_Length_Gets_Value_From_Last_Two_RA_Bytes)
{
	mix.execute(MakeSTA(dest_address, WordField{2, 3}));
	ASSERT_EQ(mix.ra().byte(5), dest_word().byte(3));
	ASSERT_EQ(mix.ra().byte(4), dest_word().byte(2));

	ASSERT_EQ(dest_word().sign(), Sign::Negative);
	ASSERT_EQ(dest_word().byte(1), 1);
	ASSERT_EQ(dest_word().byte(2), 9);
	ASSERT_EQ(dest_word().byte(3), 10);
	ASSERT_EQ(dest_word().byte(4), 4);
	ASSERT_EQ(dest_word().byte(5), 5);
}

// STA 2000(0:1)
TEST_F(STA_TAOCP_Book_Test, STA_With_One_Sign_ByteField_Gets_Value_From_5_RA_Byte_And_Its_Sign)
{
	mix.execute(MakeSTA(dest_address, WordField{0, 1}));
	ASSERT_EQ(mix.ra().byte(5), dest_word().byte(1));

	ASSERT_EQ(dest_word().sign(), Sign::Positive);
	ASSERT_EQ(dest_word().byte(1), 10);
	ASSERT_EQ(dest_word().byte(2), 2);
	ASSERT_EQ(dest_word().byte(3), 3);
	ASSERT_EQ(dest_word().byte(4), 4);
	ASSERT_EQ(dest_word().byte(5), 5);
}
