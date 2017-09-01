#include <mixal_parse/parsers_utils.h>

#include <gtest_all.h>

using namespace mixal_parse;

namespace {

std::string MakeTooLongSymbol()
{
	return std::string(2 * k_max_symbol_length, 'A');
}

std::string MakeTooLongNumber()
{
	return std::string(2 * k_max_symbol_length, '1');
}

} // namespace

TEST(ParsersUtils, Too_Long_Symbols_Are_Not_Valid)
{
	ASSERT_FALSE(IsSymbol(MakeTooLongSymbol()));
}

TEST(ParsersUtils, Too_Long_Numbers_Are_Not_Valid)
{
	ASSERT_FALSE(IsNumber(MakeTooLongNumber()));
}

TEST(ParsersUtils, Number_Is_Not_Symbol)
{
	ASSERT_FALSE(IsSymbol("00012"));
}

TEST(ParsersUtils, String_With_At_Least_One_Char_Is_Symbol)
{
	ASSERT_TRUE(IsSymbol("00012X"));
}

TEST(ParsersUtils, Symbol_Is_Basic_Expression)
{
	ASSERT_TRUE(IsSymbol("XXXE"));
	ASSERT_TRUE(IsBasicExpression("XXXE"));
}

TEST(ParsersUtils, Number_Is_Basic_Expression)
{
	ASSERT_TRUE(IsNumber("123456789"));
	ASSERT_TRUE(IsBasicExpression("123456789"));
}

TEST(ParsersUtils, Current_Address_Char_Is_Basic_Expression)
{
	ASSERT_TRUE(IsCurrentAddressSymbol("*"));
	ASSERT_TRUE(IsBasicExpression("*"));
}
