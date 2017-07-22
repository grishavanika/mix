#pragma once

#if !defined(_UNICODE) || !defined(UNICODE)
#  error This project should be configured with UNICODE turned on
#endif

#if defined(NDEBUG)
#  define NN_RELEASE 1
#else
#  define NN_DEBUG 1
#endif


