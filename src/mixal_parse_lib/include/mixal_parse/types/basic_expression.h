#pragma once
#include <mixal_parse/types/number.h>
#include <mixal_parse/types/label.h>


namespace mixal_parse {

class BasicExpression
{
public:
	enum class Kind
	{
		Unknown,
		Number,
		Symbol,
		CurrentAddress,
	};

	BasicExpression();
	BasicExpression(const std::string_view& str);
	BasicExpression(const char* str);
	BasicExpression(const std::string_view& str, Kind kind);

	Number as_number() const;
	Symbol as_symbol() const;
	std::string_view as_current_address() const;

	bool is_number() const;
	bool is_symbol() const;
	bool is_current_address() const;

	Kind kind() const;
	std::string_view data() const;

	bool is_valid() const;

private:
	static Kind KindFromString(const std::string_view& str);

private:
	std::string_view data_;
	Kind kind_;
};

bool operator==(const BasicExpression& lhs, const BasicExpression& rhs);

} // namespace mixal_parse

///////////////////////////////////////////////////////////////////////////////

#include <cassert>

namespace mixal_parse {

inline BasicExpression::BasicExpression()
	: BasicExpression{{}, Kind::Unknown}
{
}

inline BasicExpression::BasicExpression(const std::string_view& str)
	: BasicExpression{str, KindFromString(str)}
{
}

inline BasicExpression::BasicExpression(const char* str)
	: BasicExpression{std::string_view{str}}
{
}

inline BasicExpression::BasicExpression(const std::string_view& str, Kind kind)
	: data_{str}
	, kind_{kind}
{
}

inline Number BasicExpression::as_number() const
{
	assert(is_number());
	return data_;
}

inline Symbol BasicExpression::as_symbol() const
{
	assert(is_symbol());
	return Symbol::FromString(data_);
}

inline std::string_view BasicExpression::as_current_address() const
{
	assert(is_current_address());
	return data_;
}

inline bool BasicExpression::is_number() const
{
	return (kind_ == Kind::Number);
}

inline bool BasicExpression::is_symbol() const
{
	return (kind_ == Kind::Symbol);
}

inline bool BasicExpression::is_current_address() const
{
	return (kind_ == Kind::CurrentAddress);
}

inline BasicExpression::Kind BasicExpression::kind() const
{
	return kind_;
}

inline std::string_view BasicExpression::data() const
{
	return data_;
}

inline bool BasicExpression::is_valid() const
{
	return (kind_ != Kind::Unknown) && !data_.empty();
}

inline /*static*/ BasicExpression::Kind BasicExpression::KindFromString(
	const std::string_view& str)
{
	if (IsCurrentAddressSymbol(str))
	{
		return Kind::CurrentAddress;
	}
	else if (IsNumber(str))
	{
		return Kind::Number;
	}
	else if (IsSymbol(str))
	{
		return Kind::Symbol;
	}

	return BasicExpression::Kind::Unknown;
}

inline bool operator==(const BasicExpression& lhs, const BasicExpression& rhs)
{
	return (lhs.data() == rhs.data());
}

} // namespace mixal_parse


