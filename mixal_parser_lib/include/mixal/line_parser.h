#pragma once
#include <mixal/label_parser.h>
#include <mixal/operation_parser.h>

#include <optional>

namespace mixal {

class LineParser final :
	public IParser
{
public:
	const std::string_view* comment() const;
	const LabelParser* label() const;
	const OperationParser* operation() const;
	std::string_view address_str() const;

private:
	virtual void do_parse(std::string_view line) override;
	virtual void do_clear() override;

private:
	std::optional<std::string_view> comment_;
	std::optional<LabelParser> label_;
	std::optional<OperationParser> operation_;
	std::string_view address_str_;
};

} // namespace mixal

