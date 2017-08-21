#pragma once
#include <mixal/config.h>
#include <mixal/types.h>

namespace mixal {

MIXAL_LIB_EXPORT
Word CalculateUnaryOperation(const UnaryOperation& op, Word value);

MIXAL_LIB_EXPORT
Word CalculateBinaryOperation(const BinaryOperation& op, Word lhs, Word rhs);

MIXAL_LIB_EXPORT
Word CalculateOptionalUnaryOperation(const std::optional<UnaryOperation>& op, Word value);

} // namespace mixal

