#pragma once
#include <mixal/parser.h>
#include <mixal/operation_id.h>

namespace mixal {

class OperationParser final :
	public IParser
{
public:
	virtual void parse(std::string_view str) override;
	virtual std::string_view str() const override;

	bool is_pseudo_operation() const;
	bool is_native_operation() const;

	OperationId id() const;

private:
	OperationId id_{OperationId::Unknown};
	std::string_view str_;
};

} // namespace mixal
