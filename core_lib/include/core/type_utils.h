#pragma once
#include <type_traits>

namespace type_utils {

// "Transformation Trait uncvref": http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0550r0.pdf
template<typename T>
using uncvref =
	typename std::remove_cv<
		typename std::remove_reference<T>::type>::type;

} // namespace type_utils


