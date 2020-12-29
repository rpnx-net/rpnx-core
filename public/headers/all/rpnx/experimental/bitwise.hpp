/*
 * rpnx bitwise functions
 * C++20 includes <bit>, but C++20 isn't available much yet.
 * Plus, there are a lot of omissions from <bit> which I'd like to implement.
 */
#ifndef RPNXCORE_BITWISE_HPP
#define RPNXCORE_BITWISE_HPP

#include <cstddef>
#include <cstdint>
#include <climits>

#include "rpnx/experimental/cpuarchinfo.hpp"

#ifdef _MSC_VER
// When using MSC we should have access to #include <intrin.h>

#if __has_include(<intrin.h>)
#define RPNX_HAVE_BUILTIN_MSC_INTRINSICS
#endif

#endif

#ifdef __GNUC__
#define RPNX_HAVE_BUILTIN_GCC_BITWISE
#endif
#ifdef __clang__
#define RPNX_HAVE_BUILTIN_GCC_BITWISE
#endif

#ifdef __clang__
#define RPNX_HAVE_BUILTIN_CLANG_BITWISE
#endif

namespace rpnx
{

    template <typename T>
    inline constexpr int countl_zero(T v) noexcept
    {
        static_assert(std::is_unsigned_v<T>);
        int c = 0;
        for (int i = sizeof(T)*CHAR_BIT; i != 0; i--)
        {
            if ( ((T(1) << (i-1)) & v))
            {
                break;
            }
            c++;
        }
        return c;
    }


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

#ifdef RPNX_HAVE_BUILTIN_CLANG_BITWISE
    // hello
#if UINT_MAX == 65535ull
inline constexpr int bit_reverse(unsigned int x)
{
    return __builtin_bitreverse16(x);
}
#elif UINT_MAX == 4294967295ull
    inline  int bit_reverse(unsigned int x)
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

#endif

#ifdef RPNX_HAVE_BUILTIN_GCC_BITWISE
    inline constexpr int countl_zero(unsigned int x)
    {
        if (x == 0) return sizeof(x)*CHAR_BIT;
        return __builtin_clz(x);
    }

    inline constexpr int countl_zero(unsigned long x)
    {
        if (x == 0l) return sizeof(x)*CHAR_BIT;
        return __builtin_clzl(x);
    }

    inline constexpr int countl_zero(unsigned long long x)
    {
        if (x == 0l) return sizeof(x)*CHAR_BIT;
        return __builtin_clzll(x);
    }
#endif

#ifdef RPNX_HAVE_BUILTIN_MSC_INTRINSICS
#if RPNX_CPU_ARCH == RPNX_CPU_ARCH_X64 || RPNX_CPU_ARCH == RPNX_CPU_ARCH_X86 || RPNX_CPU_ARCH == RPNX_CPU_ARCH_ARM || RPNX_CPU_ARCH == RPNX_CPU_ARCH_ARM64
#if false
    // thid doesn't work because _BitScanReverse isn't constexpr
    template <>
    inline constexpr int countl_zero<unsigned int>(unsigned int t)
    {
        unsigned long index = 32;
        if (! _BitScanReverse( &index, t) ) return 32;
        return index;
    }
#endif
#endif



#endif



    template <typename T>
    inline int countl_one(T v) noexcept
    {
        return countl_zero(~v);
    }

    template <typename T>
    inline constexpr T bit_floor(T t)
    {
        static_assert(std::is_unsigned_v<T>);
        return T(1) << (CHAR_BIT*sizeof(T) - countl_zero(t) -1);
    }
    static_assert(bit_floor(std::uint32_t(3)) == 2);
    static_assert(bit_floor(std::uint32_t(2)) == 2);
    static_assert(bit_floor(std::uint32_t(9)) == 8);

    static_assert(countl_zero(std::uint16_t(1)) == 15);
    static_assert(countl_zero(std::uint32_t(1)) == 31);
    static_assert(countl_zero(std::uint64_t(1)) == 63);

}

#endif // RPNXCORE_BITWISE_HPP
