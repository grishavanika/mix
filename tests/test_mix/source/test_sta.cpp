#include <mix/computer.h>
#include <mix/commands_factory.h>

#include <gtest/gtest.h>

using namespace mix;

namespace {

class STATest : public ::testing::Test
{
private:
	void SetUp() override
	{
		dest_address = 2000;
		dest_cell.set_sign(Sign::Negative);
		dest_cell.set_byte(1, Byte{1});
		dest_cell.set_byte(2, Byte{2});
		dest_cell.set_byte(3, Byte{3});
		dest_cell.set_byte(4, Byte{4});
		dest_cell.set_byte(5, Byte{5});

		mix.set_memory(static_cast<std::size_t>(dest_address), dest_cell);

		Register ra;
		ra.set_sign(Sign::Positive);
		ra.set_byte(1, Byte{6});
		ra.set_byte(2, Byte{7});
		ra.set_byte(3, Byte{8});
		ra.set_byte(4, Byte{9});
		ra.set_byte(5, Byte{10});

		mix.set_ra(ra);
	}

protected:
	Computer mix;
	Word dest_cell;
	int dest_address;
};
} // namespace

TEST_F(STATest, Default_STA_Stores_All_Word)
{
	mix.execute(make_sta(dest_address));
	ASSERT_EQ(mix.ra(), mix.memory(static_cast<std::size_t>(dest_address)));
}

TEST_F(STATest, XXXX)
{
	mix.execute(make_sta(dest_address, Field{2, 2}));
	ASSERT_EQ(mix.ra().byte(5), mix.memory(static_cast<std::size_t>(dest_address)).byte(2));
}

TEST_F(STATest, XXXX1)
{
	mix.execute(make_sta(dest_address, Field{2, 3}));
	ASSERT_EQ(mix.ra().byte(5), mix.memory(static_cast<std::size_t>(dest_address)).byte(3));
	ASSERT_EQ(mix.ra().byte(4), mix.memory(static_cast<std::size_t>(dest_address)).byte(2));
}

