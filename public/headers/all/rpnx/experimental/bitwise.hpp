//
// Created by rnicholl on 12/24/20.
//

#ifndef RPNXCORE_BITWISE_HPP
#define RPNXCORE_BITWISE_HPP

#include <cstddef>
#include <cstdint>
#include <climits>

#if __has_include(<bit>)
#include <bit>
#endif

namespace rpnx
{

    template <typename T>
    inline int countl_zero(T v) noexcept
    {
        static_assert(std::is_unsigned_v<T>);
        int c = 0;
        for (int i = sizeof(T)*CHAR_BIT; i != 0; i--)
        {
            if ( ((1 << (i-1)) & v))
            {
                break;
            }
            c++;
        }
        return c;
    }

    template <typename T>
    T bit_floor(T t)
    {
        static_assert(std::is_unsigned_v<T>);

        if (t == std::numeric_limits<T>::max()) return (T(1) << (CHAR_BIT*sizeof(T) - 1));
        T val = t+1;
        val |= val >> 1;
        val |= val >> 2;
        val |= val >> 4;
        val |= val >> 8;
        if constexpr (sizeof(T)*CHAR_BIT > 8)
        {
            val |= val >> 8;
        }
        if constexpr (sizeof(T)*CHAR_BIT > 16)
        {
            val |= val >> 16;
        }
        if constexpr (sizeof(T)*CHAR_BIT > 32)
        {
            val |= val >> 32;
        }
        if constexpr (sizeof(T)*CHAR_BIT > 64)
        {
            val |= val >> 64;
        }
        val = val + 1;
        return val;

    }

#ifdef __GNUC__
#define RPNX_HAVE_BUILTIN_GCC_BITWISE
#endif
#ifdef __clang__
#define RPNX_HAVE_BUILTIN_GCC_BITWISE
#endif

#ifdef __clang__
#define RPNX_HAVE_BUILTIN_CLANG_BITWISE
#endif

#ifdef RPNX_HAVE_BUILTIN_CLANG_BITWISE
    // hello
#if UINT_MAX == 65535ull
inline int bit_reverse(unsigned int x)
{
    return __builtin_bitreverse16(x);
}
#elif UINT_MAX == 4294967295ull
    inline int bit_reverse(unsigned int x)
    {
        return __builtin_bitreverse32(x);
    }
#elif UINT_MAX == 18446744073709551615ull
inline int bit_reverse(unsigned int x)
{
    return __builtin_bitreverse64(x);
}
#endif

#if ULONG_MAX == 65535ull
inline int bit_reverse(unsigned long x)
{
    return __builtin_bitreverse16(x);
}
#elif ULONG_MAX == 4294967295ull
    inline int bit_reverse(unsigned long x)
    {
        return __builtin_bitreverse32(x);
    }
#elif ULONG_MAX == 18446744073709551615ull
inline int bit_reverse(unsigned long x)
{
    return __builtin_bitreverse64(x);
}
#endif

#if ULONGLONG_MAX == 65535ull
inline int bit_reverse(unsigned long long x)
{
    return __builtin_bitreverse16(x);
}
#elif ULONGLONG_MAX == 4294967295ull
    inline int bit_reverse(unsigned long long x)
    {
        return __builtin_bitreverse32(x);
    }
#elif ULONGLONG_MAX == 18446744073709551615ull
inline int bit_reverse(unsigned long long x)
{
    return __builtin_bitreverse64(x);
}

#endif

#else
    template <typename T>
    inline int bit_reverse(T t)
    {
        T result = 0;
        for (int i = 0; i < sizeof(T)*CHAR_BIT; i++)
        {
            if ((T(1) << i) & t) result |= (T(1) << (sizeof(T)*CHAR_BIT - i - 1) );
        }
        return result;
    }
#endif

#ifdef RPNX_HAVE_BUILTIN_GCC_BITWISE
    inline int countl_zero(unsigned int x)
    {
        if (x == 0) return sizeof(x)*CHAR_BIT;
        return __builtin_clz(x);
    }

    inline int countl_zero(unsigned long x)
    {
        if (x == 0l) return sizeof(x)*CHAR_BIT;
        return __builtin_clzl(x);
    }

    inline int countl_zero(unsigned long long x)
    {
        if (x == 0l) return sizeof(x)*CHAR_BIT;
        return __builtin_clzll(x);
    }
#endif

    template <typename T>
    inline int countl_one(T v) noexcept
    {
        return countl_zero(~v);
    }
}

#endif // RPNXCORE_BITWISE_HPP
