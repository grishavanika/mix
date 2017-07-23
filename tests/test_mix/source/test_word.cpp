#include <mix/word.h>
#include <mix/word_field.h>

#include <gtest/gtest.h>

using namespace mix;

TEST(Word, DefaultWordIsPositiveWithAllBytesZero)
{
	const Word w;
	ASSERT_EQ(w.sign(), Sign::Positive);
	const Byte zero{0};
	for (std::size_t i = 1; i <= Word::k_bytes_count; ++i)
	{
		ASSERT_EQ(w.byte(i), zero) << "Index: " << i;
	}
}

TEST(Word, SetBytePositiveValueWithByteFieldSetsOnlyExpectedByte)
{
	Word w;
	const Byte expected_byte{42};
	const int value = expected_byte.cast_to<int>();

	for (std::size_t byte_index = 1; byte_index <= Word::k_bytes_count; ++byte_index)
	{
		w.set_sign(Sign::Negative);

		const WordField field{byte_index, byte_index};
		w.set_value(value, field, true/*overwrite sign*/);
		ASSERT_EQ(w.byte(byte_index), expected_byte)
			<< "Index: " << byte_index;
		ASSERT_EQ(w.sign(), Sign::Positive);
	}
}

TEST(Word, SetByteNegativeValueWithByteFieldSetsExpectedByteAndSign)
{
	Word w;
	const Byte expected_byte{42};
	const int value = -expected_byte.cast_to<int>();

	for (std::size_t byte_index = 1; byte_index <= Word::k_bytes_count; ++byte_index)
	{
		w.set_sign(Sign::Positive);

		const WordField field{byte_index, byte_index};
		w.set_value(value, field, true/*overwrite sign*/);
		ASSERT_EQ(w.byte(byte_index), expected_byte)
			<< "Index: " << byte_index;
		ASSERT_EQ(w.sign(), Sign::Negative);
	}
}

TEST(Word, SetByteNegativeValueWithByteFieldAndIgnoreSignFlagSetsOnlyExpectedByte)
{
	Word w;
	const Byte expected_byte{42};
	const int value = -expected_byte.cast_to<int>();

	for (std::size_t byte_index = 1; byte_index <= Word::k_bytes_count; ++byte_index)
	{
		const WordField field{byte_index, byte_index};
		w.set_value(value, field, false/*ignore sign*/);
		ASSERT_EQ(w.byte(byte_index), expected_byte)
			<< "Index: " << byte_index;
		ASSERT_EQ(w.sign(), Sign::Positive);
	}
}

TEST(Word, SetByteNegativeValueWithSignFieldAndIgnoreSignFlagSetsSign)
{
	Word w;
	const WordField sign_field{0, 0};
	w.set_value(-1, sign_field, false/*ignore sign*/);
	ASSERT_EQ(w.sign(), Sign::Negative);
	w.set_value(1, sign_field, false/*ignore sign*/);
	ASSERT_EQ(w.sign(), Sign::Positive);
}

TEST(Word, SetValueForTwoBytesWithSignLeadsToSameBytesContentButDifferentSigns)
{
	const WordField field{0, 2};

	Word w1;
	w1.set_value(-2000, field, false/*ignore sign*/);
	ASSERT_EQ(w1.sign(), Sign::Negative);

	Word w2;
	w2.set_value(2000, field, false/*ignore sign*/);
	ASSERT_EQ(w2.sign(), Sign::Positive);

	ASSERT_EQ(w1.byte(1), w2.byte(1));
	ASSERT_EQ(w1.byte(2), w2.byte(2));

	ASSERT_EQ(-2000, w1.value());
	ASSERT_EQ(2000, w2.value());

	ASSERT_EQ(-2000, w1.value(field));
	ASSERT_EQ(2000, w2.value(field));
}

TEST(Word, SetValueForDifferentIndexesDoesNotChangesBytesContent)
{
	Word w;
	w.set_value(2000, WordField{1, 2}, false/*ignore sign*/);
	w.set_value(2000, WordField{3, 4}, false/*ignore sign*/);

	ASSERT_EQ(w.byte(1), w.byte(3));
	ASSERT_EQ(w.byte(2), w.byte(4));
}

TEST(Word, SetMaxValueLeadsToAllBytesMax)
{
	Word w;
	int value = static_cast<int>(Word::k_max_abs_value);
	w.set_value(value);
	ASSERT_EQ(value, w.value());

	for (std::size_t i = 1; i <= Word::k_bytes_count; ++i)
	{
		ASSERT_EQ(w.byte(i), Byte::max()) << "Index: " << i;
	}
}
