#pragma once
#include <mixal/parsers_utils.h>

#include <gtest/gtest.h>

template<typename Parser>
class ParserTest :
	public ::testing::Test
{
protected:
	void parse(std::string_view s)
	{
		const auto pos = parser_.parse_stream(s);
		ASSERT_FALSE(mixal::IsInvalidStreamPosition(pos));
		rest_of_parsed_stream_ = s.substr(pos);
	}

	void parse_error(std::string_view s)
	{
		const auto pos = parser_.parse_stream(s);
		ASSERT_TRUE(mixal::IsInvalidStreamPosition(pos));
	}

	void reminder_stream_is(std::string_view rest_of_stream)
	{
		ASSERT_EQ(rest_of_stream, rest_of_parsed_stream_);
	}

protected:
	Parser parser_;
	std::string_view rest_of_parsed_stream_;
};


