#pragma once
#include <mixal/parser.h>
#include <mixal/operation_id.h>

namespace mixal {

class MIXAL_PARSER_LIB_EXPORT OperationParser final :
	public IParser
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

} // namespace mixal
