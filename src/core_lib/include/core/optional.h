#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
#include <optional>
#elif __has_include(<experimental/optional>)
#include <experimental/optional>

namespace std {
using optional = experimental::optional;
} // namespace std

#else
#include <optional>
#endif