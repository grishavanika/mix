#pragma once
#include <core/string.h>

namespace mixal {

using UnaryOperation = std::string_view;
using BinaryOperation = std::string_view;
using BasicExpression = std::string_view;

extern const std::size_t k_max_symbol_length;
extern const std::size_t k_max_operation_str_length;
extern const char k_current_address_marker;

extern const UnaryOperation k_unary_operations[];
extern const BinaryOperation k_binary_operations[];

bool IsSymbol(const std::string_view& str);
bool IsNumber(const std::string_view& str);
bool IsCurrentAddressSymbol(const std::string_view& str);
bool IsBasicExpression(const std::string_view& str);

bool IsUnaryOperationBegin(char ch);
bool IsNumberBegin(char ch);
bool IsSymbolBegin(char ch);
bool IsBasicExpressionBegin(char ch);

bool IsUnaryOperationChar(char ch);
bool IsBinaryOperationChar(char ch);
bool IsNumberChar(char ch);
bool IsSymbolChar(char ch);
bool IsBasicExpressionChar(char ch);

bool IsCompletedUnaryOperation(const std::string_view& str);
bool IsCompletedBinaryOperation(const std::string_view& str);
bool IsCompletedBasicExpression(const std::string_view& str);

} // namespace mixal

