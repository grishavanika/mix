#pragma once
#include <core/string.h>

namespace mixal {

using UnaryOperation = std::string_view;
using BinaryOperation = std::string_view;
using BasicExpression = std::string_view;
using LocalSymbolId = int;

extern const std::size_t k_max_symbol_length;
extern const std::size_t k_max_operation_str_length;
extern const char k_current_address_marker;
extern const LocalSymbolId k_max_local_symbol_id;

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

bool IsValidLocalSymbolId(LocalSymbolId id);

// Both functions return position of first non-space character or `str.size()` otherwice
std::size_t SkipLeftWhiteSpaces(const std::string_view& str, std::size_t offset = 0);
std::size_t ExpectFirstNonWhiteSpaceChar(char ch, const std::string_view& str, std::size_t offset = 0);

std::size_t InvalidStreamPosition();
bool IsInvalidStreamPosition(std::size_t pos);

} // namespace mixal

