#pragma once
#include <mixal/config.h>
#include <mixal/parsers_utils.h>

namespace mixal {

class MIXAL_PARSER_LIB_EXPORT ParserBase
{
public:
	std::size_t parse_stream(std::string_view str, std::size_t offset = 0);
	
	bool is_valid() const;
	std::string_view str() const;

protected:
	ParserBase() = default;
	~ParserBase() = default;

	virtual void do_clear() = 0;
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) = 0;

private:
	void clear();

private:
	std::string_view str_;
	bool is_valid_{false};
};

inline std::size_t ParserBase::parse_stream(std::string_view str, std::size_t offset /*= 0*/)
{
	clear();

	if (offset > str.size())
	{
		return InvalidStreamPosition();
	}

	const auto pos = do_parse_stream(str, offset);
	if (IsInvalidStreamPosition(pos))
	{
		// Parser is in undetermined state.
		// Put back in default state for free
		clear();
		return InvalidStreamPosition();
	}

	is_valid_ = true;
	str_ = str.substr(offset, pos - offset);
	return pos;
}

inline void ParserBase::clear()
{
	is_valid_ = false;
	do_clear();
}

inline bool ParserBase::is_valid() const
{
	return is_valid_;
}

inline std::string_view ParserBase::str() const
{
	return str_;
}

} // namespace mixal

