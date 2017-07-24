#include <mix/word_field.h>
#include <mix/word.h>

#include <gtest/gtest.h>

using namespace mix;

TEST(WordField, Only_Adjacent_Indexes_CanBeUsed)
{
	ASSERT_NO_THROW({
		WordField(1, 2);
		WordField(0, 2);
		WordField(3, 5);
		WordField(0, 0);
	});

	ASSERT_THROW({
		WordField(2, 1);
	}, std::logic_error);
}

TEST(WordField, Max_Right_Index_Is_Words_Bytes_Count)
{
	ASSERT_NO_THROW({
		WordField(0, Word::k_bytes_count);
	});

	ASSERT_THROW({
		WordField(0, 6);
	}, std::logic_error);
}

TEST(WordField, BytesConvertions_Do_Not_Throw)
{
	WordField f{0, 5};
	const auto byte = f.to_byte();
	const auto field_from_byte = WordField::FromByte(byte);
	ASSERT_EQ(f, field_from_byte);
}

TEST(WordField, Field_With_Zero_Index_Affects_To_Sign_Properties)
{
	WordField sign_only_field{0, 0};
	ASSERT_TRUE(sign_only_field.has_only_sign());
	ASSERT_TRUE(sign_only_field.includes_sign());

	WordField field_with_bytes{0, 1};
	ASSERT_FALSE(field_with_bytes.has_only_sign());
	ASSERT_TRUE(field_with_bytes.includes_sign());

	WordField field_without_bytes{1, 1};
	ASSERT_FALSE(field_without_bytes.has_only_sign());
	ASSERT_FALSE(field_without_bytes.includes_sign());
}

TEST(WordField, SignOnly_Field_Has_Zero_Bytes_Count)
{
	WordField sign_only_field{0, 0};
	ASSERT_EQ(0u, sign_only_field.bytes_count());

	WordField field_with_bytes{0, 1};
	ASSERT_NE(0u, field_with_bytes.bytes_count());
}

TEST(WordField, Field_With_Non_Zero_Index_Has_Non_Zero_Bytes_Count)
{
	ASSERT_EQ(5u, WordField(0, 5).bytes_count());
	ASSERT_EQ(5u, WordField(1, 5).bytes_count());
	ASSERT_EQ(4u, WordField(2, 5).bytes_count());
	ASSERT_EQ(3u, WordField(3, 5).bytes_count());
	ASSERT_EQ(2u, WordField(4, 5).bytes_count());
	ASSERT_EQ(1u, WordField(5, 5).bytes_count());
	ASSERT_EQ(1u, WordField(1, 1).bytes_count());
	ASSERT_EQ(1u, WordField(2, 2).bytes_count());
	ASSERT_EQ(2u, WordField(3, 4).bytes_count());
}

TEST(WordField, SignOnly_Field_Has_Zero_Left_And_Right_Bytes_Indexes)
{
	WordField sign_only_field{0, 0};
	ASSERT_EQ(0u, sign_only_field.left_byte_index());
	ASSERT_EQ(0u, sign_only_field.right_byte_index());
}

TEST(WordField, Field_With_Non_Zero_Right_Index_Has_Valid_Left_Byte_Index)
{
	ASSERT_EQ(1u, WordField(0, 1).left_byte_index());
	ASSERT_EQ(1u, WordField(0, 2).left_byte_index());
	ASSERT_EQ(1u, WordField(0, 5).left_byte_index());
}

TEST(WordField, Field_With_Non_Zero_Indexes_Has_Valid_Byte_Indexes)
{
	{
		WordField f{1, 1};
		ASSERT_EQ(1u, f.left_byte_index());
		ASSERT_EQ(1u, f.right_byte_index());
	}

	{
		WordField f{1, 2};
		ASSERT_EQ(1u, f.left_byte_index());
		ASSERT_EQ(2u, f.right_byte_index());
	}

	{
		WordField f{5, 5};
		ASSERT_EQ(5u, f.left_byte_index());
		ASSERT_EQ(5u, f.right_byte_index());
	}
}

TEST(WordField, Shift_Bytes_Right_For_SignOnly_Field_Is_NOP)
{
	WordField sign_only_field{0, 0};
	ASSERT_EQ(sign_only_field, sign_only_field.shift_bytes_right());
}

TEST(WordField, Shift_Bytes_Right_Preserves_Bytes_Count)
{
	const WordField k_fs[] = {
		WordField{0, 0},
		WordField{0, 3},
		WordField{0, 5},
		WordField{1, 1},
		WordField{1, 3},
		WordField{1, 2},
		WordField{1, 4},
		WordField{1, 5},
		WordField{3, 5},
		WordField{5, 5},
	};
	for (auto f : k_fs)
	{
		ASSERT_EQ(f.bytes_count(), f.shift_bytes_right().bytes_count());
	}
}

TEST(WordField, Shift_Bytes_Right_Aligns_To_RightMost_Possible_Index)
{
	const struct
	{
		WordField expected;
		WordField original;
	} k_cases[] = {
		{WordField{5, 5}, WordField{0, 1}},
		{WordField{4, 5}, WordField{0, 2}},
		{WordField{5, 5}, WordField{1, 1}},
		{WordField{4, 5}, WordField{1, 2}},
		{WordField{3, 5}, WordField{1, 3}},
		{WordField{2, 5}, WordField{1, 4}},
		{WordField{5, 5}, WordField{2, 2}},
		{WordField{5, 5}, WordField{3, 3}},
		{WordField{5, 5}, WordField{4, 4}},
		{WordField{3, 5}, WordField{3, 5}},
		{WordField{1, 5}, WordField{0, 5}},
	};

	for (auto pair : k_cases)
	{
		auto shifted = pair.original.shift_bytes_right();
		ASSERT_EQ(pair.expected, shifted);
		ASSERT_EQ(Word::k_bytes_count, pair.expected.right_byte_index());
	}
}
