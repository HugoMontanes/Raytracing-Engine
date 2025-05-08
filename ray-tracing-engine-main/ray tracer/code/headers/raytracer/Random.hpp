/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <cstdint>
#include <raytracer/math.hpp>

namespace udit::raytracer
{

    class Random
    {
    private:

        uint32_t state;

    public:

        Random()
        {
            state = 0x12345678u;
        }

        Random(uint32_t seed)
        {
            state = seed;
        }

        uint32_t next_uint32 ()
        {
            state ^= state << 13;
            state ^= state >> 17;
            state ^= state << 5;
            return   state;
        }

        float value_within_01 ()
        {
            uint32_t bits = (next_uint32 () >> 9) | 0x3F800000U;

            return *reinterpret_cast< float * >(&bits) - 1.0f;
        }

        float value_within_11 ()
        {
            return value_within_01 () * 2.0f - 1.0f;
        }

        Vector2 point_inside_box_2d ()
        {
            return Vector2{ value_within_11 (), value_within_11 () };
        }

        Vector3 point_inside_box_3d ()
        {
            return Vector3{ value_within_11 (), value_within_11 (), value_within_11 () };
        }

        Vector3 point_inside_sphere ()
        {
            Vector3 point = point_inside_box_3d ();

            if (dot (point, point) < 1.f)
            {
                return point;
            }

            return point * 0.5f;
        }

        Vector3 point_on_sphere ()
        {
            return normalize (point_inside_box_3d ());
        }

    };

    extern Random random;

}
