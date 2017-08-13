#pragma once
#include <mixal/parser.h>
#include <mixal/operation_id.h>

namespace mixal {

class OperationParser final :
	public IParser
{
public:
	virtual void parse(std::string_view str) override;
	bool try_parse(std::string_view str);

	bool is_pseudo_operation() const;
	bool is_native_operation() const;

	OperationId id() const;

private:
	void clear();

private:
	OperationId id_{OperationId::Unknown};
};

} // namespace mixal
