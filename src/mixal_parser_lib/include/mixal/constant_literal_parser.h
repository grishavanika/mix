#pragma once
#include <mixal/parser.h>
#include <mixal/expression.h>

namespace mixal {

class ConstantLiteralParser :
	public IParser
{
public:
	const WordExpression& expression() const;

private:
	virtual std::size_t do_parse_stream(std::string_view str, std::size_t offset) override;
	virtual void do_clear() override;

private:
	WordExpression expression_;
};

} // namespace mixal

