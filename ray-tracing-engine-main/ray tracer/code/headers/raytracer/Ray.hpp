/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/math.hpp>

namespace udit::raytracer
{

    struct Ray
    {
        Vector3 origin;
        Vector3 direction;

        Vector3 point_at (float t) const
        {
            return origin + direction * t;
        }
    };

}
