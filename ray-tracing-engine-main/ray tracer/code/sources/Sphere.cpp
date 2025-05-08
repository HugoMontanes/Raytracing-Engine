/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <raytracer/Ray.hpp>
#include <raytracer/Sphere.hpp>

namespace udit::raytracer
{

    float Sphere::intersect (const Ray & ray, float min_t, float max_t) const
    {
        Vector3 center_origin = ray.origin - center;

        float a = dot (ray.direction, ray.direction);
        float b = dot (center_origin, ray.direction);
        float c = dot (center_origin, center_origin) - radius * radius;
        float d = b * b - a * c;

        if (d > 0.f)
        {
            d = sqrt (d);

            float t1 = (-b - d) / a;

            if (t1 > min_t && t1 < max_t) return t1;

            float t2 = (-b + d) / a;

            if (t2 > min_t && t2 < max_t) return t2;
        }

        return -1.f;
    }

}
