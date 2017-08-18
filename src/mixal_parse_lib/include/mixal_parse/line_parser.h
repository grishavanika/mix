#pragma once
#include <mixal_parse/label_parser.h>
#include <mixal_parse/operation_parser.h>
#include <mixal_parse/operation_address_parser.h>

#include <core/optional.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT LineParser final :
	public ParserBase
{
public:
	ConstOptionalRef<std::string_view> comment() const;
	ConstOptionalRef<LabelParser> label_parser() const;
	ConstOptionalRef<OperationParser> operation_parser() const;
	ConstOptionalRef<OperationAddressParser> address() const;

	bool has_only_comment() const;
	bool has_inline_comment() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

	std::size_t parse_address_str_with_comment(const std::string_view& str, std::size_t offset);
	std::size_t try_parse_line_with_label(const std::string_view& str, std::size_t offset);

	std::size_t try_parse_line_without_label(
		const std::string_view& str,
		std::size_t offset,
		bool fallback_to_label_parse = true);

private:
	std::optional<std::string_view> comment_;
	std::optional<LabelParser> label_parser_;
	std::optional<OperationParser> operation_parser_;
	std::optional<OperationAddressParser> address_;
};

} // namespace mixal_parse

