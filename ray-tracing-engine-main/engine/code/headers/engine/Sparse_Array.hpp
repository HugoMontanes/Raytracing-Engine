/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

namespace udit::engine
{

    template< typename VALUE_TYPE >
    class Sparse_Array
    {
    public:

        using          Value_Type = VALUE_TYPE;
        using Optional_Value_Type = std::optional< Value_Type >;

    private:

        static constexpr size_t segment_size  = 64;
        static constexpr size_t segment_shift =  6;
        static constexpr size_t segment_mask  = 63;

        // Se puede optimizar sustancialmente en cuanto a uso de memoria si se usa un bitmap
        // por cada segmento. También ayudaría a iterar rápidamente los elementos con valor
        // si dicha funcionalidad terminase resultando necesaria.

        using Segment         = std::array< Optional_Value_Type, segment_size >;
        using Segment_Pointer = std::unique_ptr< Segment >;

        class Collection
        {
            std::vector< Segment_Pointer > segments;

        public:

            Optional_Value_Type & operator [] (size_t element_index)
            {
                size_t segment_index = element_index >> segment_shift;

                if (segment_index >= segments.size ()) segments.resize (segment_index + 1);

                auto & segment = segments[segment_index];

                if (not segment) segment = std::make_unique< Segment > ();

                return (*segment)[element_index & segment_mask];
            }
        };

    private:

        Collection collection;

    public:

        Optional_Value_Type & operator [] (size_t index)
        {
            return collection[index];
        }

    };

}
