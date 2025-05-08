/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#ifndef MACROS_HEADER
#define MACROS_HEADER

    ////////////////////////////////////////////////////////////////////////////////////////////////

    #if defined __clang__

        #define CLANG_COMPILER   true
        #define COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)

        #if COMPILER_VERSION < 303                                           // CUIDADO CON EL NÚMERO DE VERSIÓN DEL CLANG USADO EN MAC OS

            #error "Clang 3.3 or newer required."

        #endif

    #elif defined __GNUC__

        #define GNU_COMPILER     true
        #define COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)

        #if COMPILER_VERSION < 408

            #error "GCC 4.8 or newer required."

        #endif

    #elif defined _MSC_VER

        #define MICROSOFT_COMPILER true
        #define COMPILER_VERSION  _MSC_VER

        #if COMPILER_VERSION < 1900

            #error "requires Visual C++ 2015 or newer required."

        #endif

    #else

        #error "The compiler being used is not tested/supported."

    #endif

    ////////////////////////////////////////////////////////////////////////////////////////////////

    #if MICROSOFT_COMPILER
        #define LANGUAGE_VERSION  _MSVC_LANG
    #else
        #define LANGUAGE_VERSION __cplusplus
    #endif

    #if LANGUAGE_VERSION >= 201103L
        #define CPP11_IS_SUPPORTED true
    #else
        #error "catalyst++ requires the compiler to support C++11 or avobe."
    #endif

    #if LANGUAGE_VERSION >= 201402L
        #define CPP14_IS_SUPPORTED true
    #else
        #define CPP14_IS_NOT_SUPPORTED true
    #endif

    #if LANGUAGE_VERSION >= 201703L
        #define CPP17_IS_SUPPORTED true
    #else
        #define CPP17_IS_NOT_SUPPORTED true
    #endif

    #if LANGUAGE_VERSION >= 202002L
        #define CPP20_IS_SUPPORTED true
    #else
        #define CPP20_IS_NOT_SUPPORTED true
    #endif

    ////////////////////////////////////////////////////////////////////////////////////////////////

    #if defined _WIN32 || defined _WIN64

        #define WINDOWS_OS true

    #elif defined __ANDROID__ || defined __ANDROID_NDK__

        #define ANDROID_OS true

    #elif defined __APPLE__

        #include <TargetConditionals.h>

        #if defined TARGET_OS_MAC

            #define MAC_OS true

        #endif

    #elif defined __linux__ || defined __gnu_linux__

        #define LINUX_OS true

    #else

        #error "The current target platform is not tested/supported."

    #endif

    ////////////////////////////////////////////////////////////////////////////////////////////////

    #if defined __amd64__ || defined _M_AMD64 || defined __amd64 || defined __x86_64__

        #define CPU_SUPPORTS_AMD64   true
        #define CPU_SUPPORTS_IA32    true
        #define CPU_SUPPORTS_X86     true
        #define CPU_IS_64BIT         true
        #define CPU_IS_LITTLE_ENDIAN true

    #elif defined __i386__ || defined _M_IX86 || defined __i386

        #define CPU_SUPPORTS_IA32    true
        #define CPU_SUPPORTS_X86     true
        #define CPU_IS_32BIT         true
        #define CPU_IS_LITTLE_ENDIAN true

    #elif defined __arm__ || defined __aarch64__ || defined _M_ARM

        #define CPU_SUPPORTS_ARM32   true

        #if defined __LP64__ || defined _M_X64 || defined __aarch64__ || defined __ARM_ARCH_ISA_A64

            #define CPU_SUPPORTS_ARM64 true
            #define CPU_IS_64BIT       true

        #else

            #define CPU_IS_32BIT       true

        #endif

        // Revisar Visual C++ y Clang...

        #if __ORDER_LITTLE_ENDIAN__ == __BYTE_ORDER__
            #define CPU_IS_BIG_ENDIAN    true
        #elif  __ORDER_BIG_ENDIAN__ == __BYTE_ORDER__
            #define CPU_IS_LITTLE_ENDIAN true
        #else
            #error "The current target processor endianness is not recognized or tested/supported."
        #endif

    #else

        #error "The current target processor architecture is not tested/supported."

    #endif

    ////////////////////////////////////////////////////////////////////////////////////////////////

    static_assert(sizeof(int          ) == 4, "The size of 'int' is expected to be 32 bits.");
    static_assert(sizeof(long long int) == 8, "The size of 'long long int' is expected to be 64 bits.");

    #if CPU_IS_32BIT

        static_assert(sizeof(void *) == 4, "The size of pointers is expected to be 32 bits.");

    #elif CPU_IS_64BIT

        static_assert(sizeof(void *) == 8, "The size of pointers is expected to be 64 bits.");

    #else

        #error "The CPU architecture is expected to be of either 32 or 64 bits."

    #endif

    ////////////////////////////////////////////////////////////////////////////////////////////////

    #if defined __OPTIMIZE__ || (MICROSOFT_COMPILER && not defined _DEBUG)

        #define OPTIMIZED_BUILD true

        #if not defined NDEBUG

            #define NDEBUG

        #endif

    #endif

#endif
