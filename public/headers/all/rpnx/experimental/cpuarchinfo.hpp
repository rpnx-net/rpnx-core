//
// Created by rnicholl on 12/27/2020.
//

#ifndef RPNXCORE_CPUARCHINFO_HPP
#define RPNXCORE_CPUARCHINFO_HPP

// TODO: Finish this file

#define RPNX_CPU_ARCH_X86 1
#define RPNX_CPU_ARCH_X64 2
#define RPNX_CPU_ARCH_ARM 3
#define RPNX_CPU_ARCH_ARM64 4
#define RPNX_CPU_ARCH_ALPHA 5



#if defined(__GNUC__) || defined(__clang__)

#ifdef __x86_64__
#define RPNX_CPU_ARCH RPNX_CPU_ARCH_X64
#else
#ifdef __i386__
#define RPNX_CPU_ARCH RPNX_CPU_ARCH_X86
#endif
#endif

#endif

#if defined(_MSC_VER)

#if defined(_M_AMD64) || defined(_M_X64)
#define RPNX_CPU_ARCH RPNX_CPU_ARCH_X64
#else
#ifdef _M_IX86
#define RPNX_CPU_ARCH RPNX_CPU_ARCH_X86
#endif

#endif

#endif

#ifdef RPNX_CPU_ARCH

#if RPNX_CPU_ARCH == RPNX_CPU_ARCH_X86
#define RPNX_CPU_IS_X86
#endif

#if RPNX_CPU_ARCH == RPNX_CPU_ARCH_X64
#define RPNX_CPU_IS_X64
#endif


#if RPNX_CPU_ARCH == RPNX_CPU_ARCH_ARM
#define RPNX_CPU_IS_ARM
#endif

#endif

#endif // RPNXCORE_CPUARCHINFO_HPP
