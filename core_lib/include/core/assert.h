#pragma once
#include <core/config.h>

// #TODO: temporary. Own debugging macros should be added instead
#if defined(NDEBUG) && NN_RELEASE
#  undef NDEBUG
#endif

#include <cassert>

#if !defined(NDEBUG) && NN_RELEASE
#  define NDEBUG
#endif
