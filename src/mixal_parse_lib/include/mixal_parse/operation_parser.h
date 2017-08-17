#pragma once
#include <mixal_parse/parser_base.h>
#include <mixal_parse/operation_id.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT OperationParser final :
	public ParserBase
{
public:
	bool is_mix_operation() const;
	bool is_mixal_operation() const;

	OperationId id() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

private:
	OperationId id_{OperationId::Unknown};
};

} // namespace mixal_parse
