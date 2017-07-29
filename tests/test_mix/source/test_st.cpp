#include <mix/computer.h>
#include <mix/command.h>

#include "computer_listener_mock.h"

#include <gtest/gtest.h>

using namespace mix;
using ::testing::NiceMock;

namespace {

struct STParam
{
	Register value;
	WordField field;
	Word initial_cell_value;
	Word final_cell_value;
	// Store RA to given address,
	// RX to the next one
	int start_address;
};

Register MakeRegister(Sign sign, int b1, int b2, int b3, int b4, int b5)
{
	Register r;
	r.set_sign(sign);
	r.set_byte(1, Byte{b1});
	r.set_byte(2, Byte{b2});
	r.set_byte(3, Byte{b3});
	r.set_byte(4, Byte{b4});
	r.set_byte(5, Byte{b5});
	return r;
}

IndexRegister MakeIndexRegister(Sign sign, int b4, int b5)
{
	IndexRegister ri;
	ri.set_sign(sign);
	ri.set_byte(4, Byte{b4});
	ri.set_byte(5, Byte{b5});
	return ri;
}

Register MakeWord(Sign sign, int b1, int b2, int b3, int b4, int b5)
{
	return MakeRegister(sign, b1, b2, b3, b4, b5);
}

class STTest :
	public ::testing::TestWithParam<STParam>
{
	void SetUp() override
	{
		const auto& param = GetParam();

		EXPECT_CALL(listener, on_memory_set(param.start_address + 0)).Times(2);
		EXPECT_CALL(listener, on_memory_set(param.start_address + 1)).Times(2);

		mix.set_ra(param.value);
		mix.set_rx(param.value);

		mix.set_memory(param.start_address + 0, param.initial_cell_value);
		mix.set_memory(param.start_address + 1, param.initial_cell_value);
	}

protected:
	Command make_sta() const
	{
		const auto& param = GetParam();
		return Command{24, param.start_address + 0, 0, param.field};
	}

	Command make_stx() const
	{
		const auto& param = GetParam();
		return Command{31, param.start_address + 1, 0, param.field};
	}

protected:
	NiceMock<ComputerListenerMock> listener;
	Computer mix{&listener};
};

} // namespace

TEST_P(STTest, Store_RA_RX_To_Memory)
{
	const auto& param = GetParam();

	mix.execute(make_sta());
	mix.execute(make_stx());

	ASSERT_EQ(param.final_cell_value, mix.memory(param.start_address));
	ASSERT_EQ(param.final_cell_value, mix.memory(param.start_address + 1));
}

INSTANTIATE_TEST_CASE_P(Store_Register_Rightmost_Bytes_To_Given_Memory_Cell_Field,
	STTest,
	::testing::Values(
		STParam{
			MakeRegister(Sign::Positive, 1, 2, 3, 4, 5),
			Word::MaxField(),
			MakeWord(Sign::Negative, 6, 7, 8, 9, 10),
			MakeWord(Sign::Positive, 1, 2, 3, 4, 5), 3000},
		STParam{
			MakeRegister(Sign::Positive, 1, 2, 3, 4, 5),
			Word::MaxFieldWithoutSign(),
			MakeWord(Sign::Negative, 6, 7, 8, 9, 10),
			MakeWord(Sign::Negative, 1, 2, 3, 4, 5), 3000},
		STParam{
			MakeRegister(Sign::Positive, 1, 2, 3, 4, 5),
			WordField{0, 0},
			MakeWord(Sign::Negative, 6, 7, 8, 9, 10),
			MakeWord(Sign::Positive, 6, 7, 8, 9, 10), 3000},
		STParam{
			MakeRegister(Sign::Positive, 1, 2, 3, 4, 5),
			WordField{0, 3},
			MakeWord(Sign::Negative, 6, 7, 8, 9, 10),
			MakeWord(Sign::Positive, 3, 4, 5, 9, 10), 3000},
		STParam{
			MakeRegister(Sign::Positive, 1, 2, 3, 4, 5),
			WordField{2, 3},
			MakeWord(Sign::Negative, 6, 7, 8, 9, 10),
			MakeWord(Sign::Negative, 6, 4, 5, 9, 10), 3000}
	));


namespace {

struct STIParam
{
	IndexRegister value;
	WordField field;
	Word initial_cell_value;
	Word final_cell_value;
	// Store RI[1-6] to [address + 0; address + 5]
	int start_address;
};

class STITest :
	public ::testing::TestWithParam<STIParam>
{
	void SetUp() override
	{
		const auto& param = GetParam();

		for (int i = 1; i <= 6; ++i)
		{
			EXPECT_CALL(listener, on_memory_set(param.start_address + i - 1)).Times(2);
		}
		
		for (int i = 1; i <= 6; ++i)
		{
			mix.set_ri(static_cast<std::size_t>(i), param.value);
			mix.set_memory(param.start_address + i - 1, param.initial_cell_value);
		}
	}

protected:
	Command make_sti(int i) const
	{
		const auto& param = GetParam();
		return Command{static_cast<std::size_t>(24 + i), param.start_address + i - 1, 0, param.field};
	}

protected:
	NiceMock<ComputerListenerMock> listener;
	Computer mix{&listener};
};
} // namespace

TEST_P(STITest, Store_RI_To_Memory)
{
	const auto& param = GetParam();

	for (int i = 1; i <= 6; ++i)
	{
		mix.execute(make_sti(i));
	}

	for (int i = 1; i <= 6; ++i)
	{
		ASSERT_EQ(param.final_cell_value, mix.memory(param.start_address + i - 1));
	}
}

INSTANTIATE_TEST_CASE_P(Stores_Index_Registers_With_First_3_Bytes_As_Zero,
	STITest,
	::testing::Values(
		STIParam{
			MakeIndexRegister(Sign::Positive, 4, 5),
			Word::MaxField(),
			MakeWord(Sign::Negative, 6, 7, 8, 9, 10),
			MakeWord(Sign::Positive, 0, 0, 0, 4, 5), 3000},
		STIParam{
			MakeIndexRegister(Sign::Positive, 4, 5),
			WordField{1, 2},
			MakeWord(Sign::Negative, 6, 7, 8, 9, 10),
			MakeWord(Sign::Negative, 4, 5, 8, 9, 10), 3000},
		STIParam{
			MakeIndexRegister(Sign::Positive, 4, 5),
			WordField{1, 5},
			MakeWord(Sign::Negative, 6, 7, 8, 9, 10),
			MakeWord(Sign::Negative, 0, 0, 0, 4, 5), 3000}
	));

// #TODO: STJ and STZ
