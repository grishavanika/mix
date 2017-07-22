#include <mix/word.h>

#include <gtest/gtest.h>

using namespace mix;

TEST(Word, DefaultWordIsPositiveWithAllBytesZero)
{
	const Word w;
	ASSERT_EQ(w.sign(), Sign::Positive);
	const Byte zero{0};
	for (std::size_t i = 0; i < Word::k_bytes_count; ++i)
	{
		ASSERT_EQ(w.byte(i), zero);
	}
}


