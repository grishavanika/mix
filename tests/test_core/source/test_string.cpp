#include <core/string.h>

#include <gtest/gtest.h>

using namespace core;

// Disable totally for this file for test purpose
#pragma warning(disable:4996)

TEST(Sprintf, Behaviour_Is_The_Same_As_For_C_Printf)
{
	{
		char buffer[1024];
		auto str = Sprintf("test %i", 1);
		ASSERT_EQ(sprintf(buffer, "test %i", 1), static_cast<int>(str.size()));
		ASSERT_STREQ(buffer, str.c_str());
	}

	{
		char buffer[1024];
		auto str = Sprintf("test %-*.*f complex - %s, %c", 1, 2, 3.1412356, "xxx", 'c');
		ASSERT_EQ(sprintf(buffer, "test %-*.*f complex - %s, %c", 1, 2, 3.1412356, "xxx", 'c'), static_cast<int>(str.size()));
		ASSERT_STREQ(buffer, str.c_str());
	}
}

#if !defined(_MSC_VER)
TEST(Sprintf, Result_Is_Truncated_For_Too_Big_Strings)
{
	std::string too_long(5u * 1024, 'x');
	auto str = Sprintf("%s", too_long.c_str());
	ASSERT_NE(too_long.size(), str.size());
}
#else
TEST(Sprintf, Result_Is_NOT_Truncated_For_Too_Big_Strings)
{
	std::string too_long(5u * 1024, 'x');
	auto str = Sprintf("%s", too_long.c_str());
	ASSERT_EQ(too_long, str);
}
#endif

TEST(StringWriteInto, Returns_Nullptr_When_Length_Is_Invalid_For_Resulting_String)
{
	std::string dummy;
	{
		auto result = StringWriteInto(dummy, 0);
		ASSERT_EQ(nullptr, result);
	}
	{
		auto result = StringWriteInto(dummy, 1);
		ASSERT_EQ(nullptr, result);
	}
}

TEST(StringWriteInto, Changes_String_Size_To_Requested_Size_Without_Null_At_End)
{
	std::string str;
	(void)StringWriteInto(str, 100u);
	ASSERT_EQ(99u, str.size());
}

TEST(StringWriteInto, Returns_Valid_C_String_Pointer_To_String_Buffer)
{
	std::string str;
	auto result = StringWriteInto(str, 100u);
	ASSERT_FALSE(str.empty());
	ASSERT_EQ(&str[0], result);
	
	const bool buffer_is_the_same = (strncmp(&str[0], result, 99) == 0);
	ASSERT_TRUE(buffer_is_the_same);
}
