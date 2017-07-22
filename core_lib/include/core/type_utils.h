#pragma once
#include <type_traits>

namespace type_utils {

template<typename T>
using base_type =
	typename std::remove_cv<
		typename std::remove_reference<T>::type>::type;

} // namespace type_utils


