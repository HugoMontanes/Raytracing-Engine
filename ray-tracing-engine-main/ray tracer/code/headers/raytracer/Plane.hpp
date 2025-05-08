/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/Intersectable.hpp>
#include <raytracer/math.hpp>

namespace udit::raytracer
{

    struct Plane : public Intersectable
    {
        static constexpr float epsilon = 0.00001f;

        Vector3    point;
        Vector3    normal;

        Plane(const Vector3 & given_point, const Vector3 & given_normal, Material * given_material)
        {
            point    = given_point;
            normal   = normalize (given_normal);
            material = given_material;
        }

        float intersect (const Ray & ray, float min_t, float max_t) const override;

        Vector3 normal_at (const Vector3 & ) const override
        {
            return normal;
        }
    };

}
