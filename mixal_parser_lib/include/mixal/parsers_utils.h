#pragma once
#include <string_view>

namespace mixal {

extern const std::size_t k_max_symbol_length;
extern const char k_current_address_marker;

bool IsSymbol(const std::string_view& str);
bool IsNumber(const std::string_view& str);
bool IsCurrentAddressSymbol(const std::string_view& str);
bool IsBasicExpression(const std::string_view& str);

} // namespace mixal

