/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <raytracer/Plane.hpp>
#include <raytracer/Ray.hpp>

namespace udit::raytracer
{

    float Plane::intersect (const Ray & ray, float min_t, float max_t) const
    {
        float denominator = dot (Plane::normal, ray.direction);

        if (fabs (denominator) > epsilon)
        {
            float t = dot (Plane::point - ray.origin, Plane::normal) / denominator;

            if (t > min_t && t < max_t) return t;
        }

        return -1.f;
    }

}
