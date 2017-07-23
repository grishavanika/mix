#include <mix/computer.h>
#include <mix/commands_factory.h>

#include <gtest/gtest.h>

using namespace mix;

namespace {

// Common format of test Word
Word make_test_word(int address, int b3, int b4, int b5)
{
	Word data;
	data.set_value(address, WordField{0, 2});
	data.set_byte(3, Byte{b3});
	data.set_byte(4, Byte{b4});
	data.set_byte(5, Byte{b5});
	return data;
}

class LDATest : public ::testing::Test
{
private:
	void SetUp() override
	{
		source_address = 2000;
		source_cell = make_test_word(-80, 3, 5, 4);
		mix.set_memory(static_cast<int>(source_address), source_cell);
	}

protected:
	Computer mix;
	Word source_cell;
	int source_address;
};

} // namespace

TEST_F(LDATest, Default_LDA_Gets_All_Word)
{
	mix.execute(make_lda(source_address));
	ASSERT_EQ(source_cell, mix.ra());
}

TEST_F(LDATest, LDA_WithoutSign_Gets_Word_WithoutSign)
{
	mix.execute(make_lda(source_address, WordField{1, 5}));
	
	ASSERT_EQ(Sign::Positive, mix.ra().sign());
	
	for (size_t i = 1; i <= 5; ++i)
	{
		ASSERT_EQ(source_cell.byte(i), mix.ra().byte(i))
			<< "Index: " << i;
	}
}

TEST_F(LDATest, LDA_With_OnlySign_Gets_Only_Sign)
{
	mix.execute(make_lda(source_address, WordField{0, 0}));

	ASSERT_EQ(Sign::Negative, mix.ra().sign());
}

TEST_F(LDATest, LDA_With_FirstByte_Is_ShiftedRight)
{
	mix.execute(make_lda(source_address, WordField{1, 1}));

	ASSERT_EQ(Sign::Positive, mix.ra().sign());

	ASSERT_EQ(source_cell.byte(1), mix.ra().byte(5));
}

TEST_F(LDATest, LDA_With_CustomFild_ShiftedRight)
{
	mix.execute(make_lda(source_address, WordField{4, 4}));

	ASSERT_EQ(Sign::Positive, mix.ra().sign());

	ASSERT_EQ(source_cell.byte(4), mix.ra().byte(5));
}

TEST_F(LDATest, LDA_With_CustomFild_MapsToTheSameField_If_CantBe_ShiftedRight)
{
	mix.execute(make_lda(source_address, WordField{3, 5}));

	ASSERT_EQ(Sign::Positive, mix.ra().sign());

	ASSERT_EQ(source_cell.byte(3), mix.ra().byte(3));
	ASSERT_EQ(source_cell.byte(4), mix.ra().byte(4));
	ASSERT_EQ(source_cell.byte(5), mix.ra().byte(5));
}

TEST_F(LDATest, LDA_With_CustomFild_WithSign_ShiftedRight)
{
	mix.execute(make_lda(source_address, WordField{0, 3}));

	ASSERT_EQ(Sign::Negative, mix.ra().sign());

	ASSERT_EQ(source_cell.byte(1), mix.ra().byte(3));
	ASSERT_EQ(source_cell.byte(2), mix.ra().byte(4));
	ASSERT_EQ(source_cell.byte(3), mix.ra().byte(5));
}


// #TODO: add tests for other registers
// #TODO: add cases when address is negative and everything is
// fixed by index register, e.g.: make_lda(-200, 1)
