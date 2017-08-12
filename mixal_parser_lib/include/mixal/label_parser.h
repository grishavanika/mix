#pragma once
#include <mixal/parser.h>

#include <optional>

namespace mixal {

// In range [0; 9]
using LocalSymbolId = unsigned char;

class LabelParser final :
	public IParser
{
public:
	static const std::size_t k_max_label_length = 10;

public:
	virtual void parse(std::string_view str) override;
	virtual std::string_view str() const override;

	bool empty() const;
	std::string_view name() const;
	bool is_local_symbol() const;
	std::optional<LocalSymbolId> local_symbol_id() const;

private:
	std::optional<LocalSymbolId> local_symbol_id_;
	std::string_view name_;
};

} // namespace mixal

