#include <mixal/translator.h>
#include <mixal/exceptions.h>

#include <mix/char_table.h>

#include <core/utils.h>

#include <gtest/gtest.h>

using namespace mixal;

TEST(TranslatorTest, Evaluates_Number_To_Word_With_Same_Integer_Value)
{
	Translator t;
	ASSERT_EQ(Word{111111}, t.evaluate(Number{"111111"}));
}

TEST(TranslatorTest, Throws_TooBigWordValueError_When_Number_Can_Not_Be_Hold_By_Word)
{
	Translator t;

	ASSERT_THROW({
		// Note: maximum possible MIXAL number: 10 digits, all 9
		t.evaluate(Number{"9999999999"});
	}, TooBigWordValueError);
}

TEST(TranslatorTest, Evaluates_Text_To_Word_Bytes_That_Corresponds_To_MIX_Char_Table)
{
	Translator t;
	Word expected_word{Word::BytesArray{0, 8, 24, 15, 4}};
	ASSERT_EQ(' ', mix::ByteToChar(0));
	ASSERT_EQ('H', mix::ByteToChar(8));
	ASSERT_EQ('U', mix::ByteToChar(24));
	ASSERT_EQ('N', mix::ByteToChar(15));
	ASSERT_EQ('D', mix::ByteToChar(4));

	ASSERT_EQ(expected_word, t.evaluate(Text{" HUND"}));
}


TEST(TranslatorTest, Throws_InvalidALFText_For_Text_With_Wrong_Length)
{
	const char k_short_text[] = "HH";
	ASSERT_LT(core::ArraySize(k_short_text), Word::k_bytes_count);

	Translator t;
	ASSERT_THROW({
		t.evaluate(Text{k_short_text});
	}, InvalidALFText);
}

TEST(TranslatorTest, Throws_InvalidALFText_For_Text_With_Invalid_MIX_Char)
{
	bool converted = true;
	(void)mix::CharToByte('"', &converted);
	ASSERT_FALSE(converted);

	Translator t;
	ASSERT_THROW({
		t.evaluate(Text{R"(""""")"});
	}, InvalidALFText);
}
