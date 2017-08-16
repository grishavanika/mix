#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
#include <optional>
#elif __has_include(<experimental/optional>)
#include <experimental/optional>

namespace std {

template<typename T>
using optional = experimental::optional<T>;

constexpr auto nullopt = experimental::nullopt;
} // namespace std

#else
#include <optional>
#endif

template<typename T>
using ConstOptionalRef = const std::optional<T>&;

