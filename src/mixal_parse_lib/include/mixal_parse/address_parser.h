#pragma once
#include <mixal_parse/parser_base.h>
#include <mixal_parse/types/address.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT AddressParser final :
	public ParserBase
{
public:
	const Address& address() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

	std::size_t try_parse_expression(const std::string_view& str, std::size_t offset);
	std::size_t try_parse_literal(const std::string_view& str, std::size_t offset);

private:
	Address address_;
};

} // namespace mixal_parse
