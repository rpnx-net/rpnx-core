#ifndef RPNX_ASSERT_HPP
#define RPNX_ASSERT_HPP

#include <assert.h>

namespace rpnx
{
    namespace detail
    {
        inline void rpnx_assert_impl(char const * str, bool value)
        {

        }
    }
}

#if defined(NDEBUG) || defined(_NDEBUG)
#if defined(__GNUC__) && defined(__cplusplus)
#define RPNX_ASSERT(x) __builtin_assume(x)
#elif defined(__clang__)
#define RPNX_ASSERT(x) __builtin_assume(x)
#elif defined(_MSC_VER)
#define RPNX_ASSERT(x) __assume(x)
#endif
#else
#define RPNX_ASSERT(x) assert(x)
#endif

#endif