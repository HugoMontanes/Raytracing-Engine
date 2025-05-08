/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#ifndef UDIT_FNV_HEADER
#define UDIT_FNV_HEADER

    //#include <string>
    //#include <cstdint>

    //namespace udit
    //{

    //    namespace internal
    //    {

    //        constinit uint32_t fnv_basis_32 = 0x811c9dc5u;
    //        constinit uint32_t fnv_prime_32 = 0x01000193u;
    //        constinit uint64_t fnv_basis_64 = 0xcbf29ce484222325u;
    //        constinit uint64_t fnv_prime_64 = 0x00000100000001b3u;

    //        template< size_t LENGTH >
    //        consteval uint32_t static_fnv32 (const char * chars)
    //        {
    //            return (static_fnv32< LENGTH - 1 > (chars) ^ chars[LENGTH - 2]) * fnv_prime_32;
    //        }

    //        template< >
    //        consteval uint32_t static_fnv32< 1 > (const char * )
    //        {
    //            return fnv_basis_32;
    //        }

    //        template< size_t LENGTH >
    //        consteval uint64_t static_fnv64 (const char * chars)
    //        {
    //            return (static_fnv64< LENGTH - 1 > (chars) ^ chars[LENGTH - 2]) * fnv_prime_64;
    //        }

    //        template< >
    //        consteval uint64_t static_fnv64< 1 > (const char * )
    //        {
    //            return fnv_basis_64;
    //        }

    //    }

    //    // -----------------------------------------------------------------------------------------

    //    /**
    //     * Implements the Fowler–Noll–Vo hash function (FNV-1a) which calculates a 32 bit hash code
    //     * from a C string literal (or array of chars) at compile time.
    //     * @tparam LENGTH Length of the string literal or array of chars.
    //     * @param  chars
    //     * @return
    //     */
    //    template< size_t LENGTH >
    //    constexpr uint32_t static_fnv32 (const char (& chars)[LENGTH])
    //    {
    //        return internal::static_fnv32< LENGTH > (chars);
    //    }

    //    // -----------------------------------------------------------------------------------------

    //    template< size_t LENGTH >
    //    constexpr uint64_t static_fnv64 (const char (& chars)[LENGTH])
    //    {
    //        return internal::static_fnv64< LENGTH > (chars);
    //    }

    //    // -----------------------------------------------------------------------------------------

    //    #if BASICS_INT_SIZE == 4

    //        template< size_t LENGTH >
    //        constexpr unsigned static_fnv (const char (& chars)[LENGTH])
    //        {
    //            return internal::static_fnv32< LENGTH > (chars);
    //        }

    //    #else

    //        template< size_t LENGTH >
    //        constexpr unsigned static_fnv (const char (& chars)[LENGTH])
    //        {
    //            return internal::static_fnv64< LENGTH > (chars);
    //        }

    //    #endif

    //    // -----------------------------------------------------------------------------------------

    //    template< size_t LENGTH >
    //    uint32_t fnv32 (const char (& chars)[LENGTH])
    //    {
    //        uint32_t hash = internal::fnv_basis_32;

    //        for (size_t index = 0; index < LENGTH; ++index)
    //        {
    //            hash ^= *(chars++);
    //            hash *= internal::fnv_prime_32;
    //        }

    //        return hash;
    //    }

    //    inline uint32_t fnv32 (const std::string & s)
    //    {
    //        uint32_t hash = internal::fnv_basis_32;

    //        for (auto c : s)
    //        {
    //            hash ^= c;
    //            hash *= internal::fnv_prime_32;
    //        }

    //        return hash;
    //    }

    //}

    //constexpr unsigned operator "" _fnv (const char * c)
    //{
    //    return c ? 1 : operator "" _fnv ("2");
    //}

    // ---------------------------------------------------------------------------------------------

    #define FNV(X)   udit::static_fnv   (#X)
    #define FNV32(X) udit::static_fnv32 (#X)
    #define FNV64(X) udit::static_fnv64 (#X)

#endif
