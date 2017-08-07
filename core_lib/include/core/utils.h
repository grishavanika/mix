#pragma once
#include <cstddef>

namespace core {

template<typename T, std::size_t N>
constexpr std::size_t ArraySize(T (&)[N])
{
	return N;
}

} // namespace core

