/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/Intersectable.hpp>

namespace udit::raytracer
{

    struct Sphere : public Intersectable
    {
        Vector3    center;
        float      radius;

        Sphere(const Vector3 & given_center, float given_radius, Material * given_material)
        {
            center   = given_center;
            radius   = given_radius;
            material = given_material;
        }

        float intersect (const Ray & ray, float min_t, float max_t) const override;

        Vector3 normal_at (const Vector3 & point) const override
        {
            return (point - center) / radius;
        }
    };

}
