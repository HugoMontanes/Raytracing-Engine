/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/Color.hpp>
#include <raytracer/math.hpp>
#include <raytracer/Sky_Environment.hpp>

namespace udit::raytracer
{

    class Skydome : public Sky_Environment
    {
        Color     sky_color;
        Color horizon_color;

    public:

        Skydome(const Color & given_sky_color, const Color & given_horizon_color)
        {
                sky_color =     given_sky_color;
            horizon_color = given_horizon_color;
        }

        Color sample (const Vector3 & direction) const
        {
            return mix (sky_color, horizon_color, 0.5f + 0.5f * direction.y);
        }

    };

}
