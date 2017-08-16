#pragma once
#include <mixal/parser.h>
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
	public IParser
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

private:
	OperationId op_;
	std::optional<MIXOpParser> mix_;
	std::optional<MIXALOpParser> mixal_;
};

} // namespace mixal
