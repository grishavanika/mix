#pragma once
#include <mixal/label_parser.h>
#include <mixal/operation_parser.h>

#include <core/optional.h>

namespace mixal {

// Given MIXAL line of code, splits it into parts:
// 1. Single comment line, if line is comment (starts with `*`)
// 2. LABEL (if any)
// 3. MIX OPERATION (if line is not comment)
// 4. ADDRESS-string after OPERATION and to the end of line or to the
// beginning of the comment (any lower case character).
// ADDRESS-string should be parsed differently depending on OPERATION

class LineParser final :
	public IParser
{
public:
	const std::string_view* comment() const;
	const LabelParser* label() const;
	const OperationParser* operation() const;
	std::string_view address_str() const;

	bool has_only_comment() const;
	bool has_inline_comment() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

	void parse_address_str_with_comment(std::string_view line);

private:
	std::optional<std::string_view> comment_;
	std::optional<LabelParser> label_;
	std::optional<OperationParser> operation_;
	std::string_view address_str_;
};

} // namespace mixal

