#pragma once
#include <gtest/gtest.h>

struct CustomTestNameSuffix
{
	template<typename ParamType>
	std::string operator()(const ::testing::TestParamInfo<ParamType>& info) const
	{
		return std::to_string(info.index);
	}
};

// Workaround for GCC error:
// `ISO C++11 requires at least one argument for the "..." in a variadic macro`
#define DEF_INSTANTIATE_TEST_CASE_P(prefix, test_case_name, generator)	\
	INSTANTIATE_TEST_CASE_P(											\
		prefix,															\
		test_case_name,													\
		generator,														\
		CustomTestNameSuffix{})

