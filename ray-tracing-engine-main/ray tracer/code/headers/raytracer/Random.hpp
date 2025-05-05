/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <random>
#include <raytracer/math.hpp>

namespace udit::raytracer
{

    class Random
    {
        using Mersenne_Twister = std::mt19937;
        using Distribution     = std::uniform_real_distribution< float >;

    private:

        Mersenne_Twister generator;

    public:

        template< float MIN, float MAX >
        float value_within ()
        {
            return Distribution(MIN, MAX) (generator);
        }

        template< float MIN, float MAX >
        Vector3 point_inside_box ()
        {
            return Vector3{ value_within< MIN, MAX > (), value_within< MIN, MAX > (), value_within< MIN, MAX > () };
        }

        template< float RADIUS >
        Vector3 point_inside_sphere ()
        {
            Vector3 point = point_inside_box < -RADIUS, +RADIUS > ();
                
            if (dot (point, point) < RADIUS *  RADIUS) 
            {
                return point;
            }

            return point_inside_box < -RADIUS / 2.f , +RADIUS / 2.f > ();
        }

        template< float RADIUS >
        Vector3 point_on_sphere ()
        {
            return normalize (point_inside_box< -RADIUS, +RADIUS > ());
        }

    };

    extern Random random;

}
