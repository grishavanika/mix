#pragma once
#include <mixal_parse/parser_base.h>
#include <mixal_parse/operation.h>

namespace mixal_parse {

class MIXAL_PARSE_LIB_EXPORT OperationParser final :
	public ParserBase
{
public:
	Operation operation() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

private:
	Operation op_;
};

} // namespace mixal_parse
