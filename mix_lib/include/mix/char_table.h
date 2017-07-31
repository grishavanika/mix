#pragma once
#include <mix/byte.h>

namespace mix {

using Char = unsigned char;

Char ByteToChar(Byte b);
Byte CharToByte(Char ch);

} // namespace mix
