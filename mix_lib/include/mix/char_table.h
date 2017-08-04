#pragma once
#include <mix/byte.h>

namespace mix {

char ByteToChar(Byte b);
Byte CharToByte(char ch);

std::size_t ByteToDecimalDigit(const Byte& byte);
Byte DecimalDigitToByte(std::size_t digit);

} // namespace mix
