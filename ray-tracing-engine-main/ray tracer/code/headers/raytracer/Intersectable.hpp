/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/declarations.hpp>
#include <raytracer/math.hpp>

namespace udit::raytracer
{

    struct Intersectable
    {
        Material * material;

    public:

        virtual float   intersect (const Ray & ray, float min_t, float max_t) const = 0;

        virtual Vector3 normal_at (const Vector3 & point) const = 0;
    };

}
