#include "precompiled.h"

using namespace mix;

namespace
{
	Register MakePositive(std::initializer_list<int> bytes)
	{
		assert(bytes.size() == 5);
		Register r;
		r.set_sign(Sign::Positive);
		for (std::size_t i = 1; i <= 5; ++i)
		{
			const int v = *(bytes.begin() + (i - 1));
			r.set_byte(i, v);
		}
		return r;
	}

	Register MakeNegative(std::initializer_list<int> bytes)
	{
		Register r = MakePositive(bytes);
		r.set_sign(Sign::Negative);
		return r;
	}
} // namespace

TEST(Shifts_TAOCP_Book_Test, RAX_shifts)
{
	Computer mix;

	mix.set_ra(MakePositive({1, 2, 3, 4, 5}));
	mix.set_rx(MakeNegative({6, 7, 8, 9, 10}));

	mix.execute(MakeSRAX(1));
	ASSERT_EQ(MakePositive({0, 1, 2, 3, 4}), mix.ra());
	ASSERT_EQ(MakeNegative({5, 6, 7, 8, 9}), mix.rx());

	mix.execute(MakeSLA(2));
	ASSERT_EQ(MakePositive({2, 3, 4, 0, 0}), mix.ra());
	ASSERT_EQ(MakeNegative({5, 6, 7, 8, 9}), mix.rx());

	mix.execute(MakeSRC(4));
	ASSERT_EQ(MakePositive({6, 7, 8, 9, 2}), mix.ra());
	ASSERT_EQ(MakeNegative({3, 4, 0, 0, 5}), mix.rx());

	mix.execute(MakeSRA(2));
	ASSERT_EQ(MakePositive({0, 0, 6, 7 ,8}), mix.ra());
	ASSERT_EQ(MakeNegative({3, 4, 0, 0, 5}), mix.rx());

	mix.execute(MakeSLC(1));
	ASSERT_EQ(MakePositive({0, 6, 7, 8, 3}), mix.ra());
	ASSERT_EQ(MakeNegative({4, 0, 0, 5, 0}), mix.rx());
}
