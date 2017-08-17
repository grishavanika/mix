#pragma once
#include <mixal/parser_base.h>
#include <mixal/operation_id.h>
#include <mixal/address_parser.h>
#include <mixal/index_parser.h>
#include <mixal/field_parser.h>
#include <mixal/w_value_parser.h>

namespace mixal {

struct MIXOpParser
{
	AddressParser address_parser;
	IndexParser index_parser;
	FieldParser field_parser;
};

struct MIXALOpParser
{
	std::optional<WValueParser> w_value_parser;
	std::optional<std::string_view> alf_text;
};

// Operation-dependent parsing of MIXAL ADDRESS column
class MIXAL_PARSER_LIB_EXPORT OperationAddressParser final :
	public ParserBase
{
public:
	explicit OperationAddressParser(OperationId operation);

	ConstOptionalRef<MIXOpParser> mix() const;
	ConstOptionalRef<MIXALOpParser> mixal() const;

	bool is_mix_operation() const;
	bool is_mixal_operation() const;

private:
	virtual void do_clear() override;
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;

	std::size_t parse_as_mix(const std::string_view& str, std::size_t offset);
	std::size_t parse_as_mixal(const std::string_view& str, std::size_t offset);

	std::size_t parse_mixal_op_as_wvalue(const std::string_view& str, std::size_t offset);
	std::size_t parse_mixal_alf_op(const std::string_view& str, std::size_t offset);

	std::size_t try_parse_alf_with_quotes(const std::string_view& str, std::size_t offset);
	std::size_t try_parse_standard_alf(const std::string_view& str, std::size_t offset);
	std::size_t set_alf_text(const std::string_view& str, std::size_t offset);

private:
	OperationId op_;
	std::optional<MIXOpParser> mix_;
	std::optional<MIXALOpParser> mixal_;
};

} // namespace mixal
