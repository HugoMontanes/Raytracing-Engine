/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <cstddef>
#include <forward_list>
#include <type_traits>
#include <vector>

namespace udit::raytracer
{

    class Memory_Pool
    {

        using Segment = std::vector< std::byte >;
        using Pool    = std::forward_list< Segment >;

    private:

        static constexpr size_t default_segment_size = 65536u;

        Pool   pool;

        size_t segment_size;
        size_t allocated;

    public:

        Memory_Pool(size_t desired_segment_size = default_segment_size)
        {
            segment_size = desired_segment_size;
            allocated    = segment_size;
        }

        template< typename TYPE,  typename ...ARGUMENTS >
        TYPE * allocate (const ARGUMENTS & ...arguments)
        {
            if (allocated + sizeof(TYPE) > segment_size)
            {
                pool.emplace_front (segment_size);

                allocated = 0;
            }

            auto object = new (pool.front ().data () + allocated) TYPE(arguments...);

            allocated += sizeof(TYPE);

            return object;
        }

    };

}
