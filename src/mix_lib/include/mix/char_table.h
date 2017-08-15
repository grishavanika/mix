#pragma once
#include <mix/config.h>
#include <mix/byte.h>

namespace mix {

MIX_LIB_EXPORT
char ByteToChar(Byte b);

MIX_LIB_EXPORT
Byte CharToByte(char ch);

MIX_LIB_EXPORT
std::size_t ByteToDecimalDigit(const Byte& byte);

MIX_LIB_EXPORT
Byte DecimalDigitToByte(std::size_t digit);

} // namespace mix
