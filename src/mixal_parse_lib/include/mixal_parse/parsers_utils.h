#pragma once
#include <mixal_parse/config.h>

#include <core/string.h>

namespace mixal_parse {

using UnaryOperation = std::string_view;
using BinaryOperation = std::string_view;

using LocalSymbolId = int;

enum class LocalSymbolKind
{
	Unknown		= -1,
	Usual		= 0,
	Here		= 'H',
	Forward		= 'F',
	Backward	= 'B',
};

MIXAL_PARSE_LIB_EXPORT
extern const std::size_t k_max_symbol_length;
MIXAL_PARSE_LIB_EXPORT
extern const std::size_t k_max_operation_str_length;
MIXAL_PARSE_LIB_EXPORT
extern const char k_current_address_marker;
MIXAL_PARSE_LIB_EXPORT
extern const LocalSymbolId k_max_local_symbol_id;
MIXAL_PARSE_LIB_EXPORT
extern const LocalSymbolId k_invalid_local_symbol_id;
MIXAL_PARSE_LIB_EXPORT
extern const UnaryOperation k_unary_operations[];
MIXAL_PARSE_LIB_EXPORT
extern const BinaryOperation k_binary_operations[];

MIXAL_PARSE_LIB_EXPORT
bool IsSymbol(const std::string_view& str);

MIXAL_PARSE_LIB_EXPORT
bool IsNumber(const std::string_view& str);

MIXAL_PARSE_LIB_EXPORT
bool IsCurrentAddressSymbol(const std::string_view& str);

MIXAL_PARSE_LIB_EXPORT
bool IsBasicExpression(const std::string_view& str);

MIXAL_PARSE_LIB_EXPORT
bool IsUnaryOperationBegin(char ch);

MIXAL_PARSE_LIB_EXPORT
bool IsNumberBegin(char ch);

MIXAL_PARSE_LIB_EXPORT
bool IsSymbolBegin(char ch);

MIXAL_PARSE_LIB_EXPORT
bool IsBasicExpressionBegin(char ch);

MIXAL_PARSE_LIB_EXPORT
bool IsUnaryOperationChar(char ch);

MIXAL_PARSE_LIB_EXPORT
bool IsBinaryOperationChar(char ch);

MIXAL_PARSE_LIB_EXPORT
bool IsNumberChar(char ch);

MIXAL_PARSE_LIB_EXPORT
bool IsSymbolChar(char ch);

MIXAL_PARSE_LIB_EXPORT
bool IsBasicExpressionChar(char ch);

MIXAL_PARSE_LIB_EXPORT
bool IsCompletedUnaryOperation(const std::string_view& str);

MIXAL_PARSE_LIB_EXPORT
bool IsCompletedBinaryOperation(const std::string_view& str);

MIXAL_PARSE_LIB_EXPORT
bool IsCompletedBasicExpression(const std::string_view& str);

MIXAL_PARSE_LIB_EXPORT
bool IsValidLocalSymbolId(LocalSymbolId id);

MIXAL_PARSE_LIB_EXPORT
bool IsLocalSymbol(const std::string_view& str);

MIXAL_PARSE_LIB_EXPORT
LocalSymbolKind ParseLocalSymbolKind(const std::string_view& str);

MIXAL_PARSE_LIB_EXPORT
LocalSymbolId ParseLocalSymbolId(const std::string_view& str);

// Both functions return position of first non-space character or `str.size()` otherwice
MIXAL_PARSE_LIB_EXPORT
std::size_t SkipLeftWhiteSpaces(const std::string_view& str, std::size_t offset = 0);

MIXAL_PARSE_LIB_EXPORT
std::size_t ExpectFirstNonWhiteSpaceChar(char ch, const std::string_view& str, std::size_t offset = 0);

MIXAL_PARSE_LIB_EXPORT
std::size_t InvalidStreamPosition();

MIXAL_PARSE_LIB_EXPORT
bool IsInvalidStreamPosition(std::size_t pos);

} // namespace mixal_parse

