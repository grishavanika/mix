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
	virtual void do_parse(std::string_view str) override;
	virtual void do_clear() override;

private:
	WordExpression expression_;
};

} // namespace mixal

