/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/Color.hpp>
#include <raytracer/declarations.hpp>

namespace udit::raytracer
{

    struct Material
    {
        virtual bool scatter (const Ray & incident_ray, Ray & scattered_ray, const Intersection & intersection, Color & attenuation) = 0;

        virtual ~Material() = default;
    };

}
