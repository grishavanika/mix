#pragma once
#include <mixal/label_parser.h>
#include <mixal/operation_parser.h>

#include <core/optional.h>

namespace mixal {

class MIXAL_PARSER_LIB_EXPORT LineParser final :
	public IParser
{
public:
	ConstOptionalRef<std::string_view> comment() const;
	ConstOptionalRef<LabelParser> label() const;
	ConstOptionalRef<OperationParser> operation() const;

	std::string_view address_str() const;

	bool has_only_comment() const;
	bool has_inline_comment() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

	std::size_t parse_address_str_with_comment(const std::string_view& str, std::size_t offset);
	std::size_t try_parse_line_without_label(const std::string_view& str, std::size_t offset);
	std::size_t try_parse_line_with_label(const std::string_view& str, std::size_t offset);

private:
	std::optional<std::string_view> comment_;
	std::optional<LabelParser> label_;
	std::optional<OperationParser> operation_;
	std::string_view address_str_;
};

} // namespace mixal

