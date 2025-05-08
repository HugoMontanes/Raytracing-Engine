/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/Color.hpp>
#include <raytracer/Intersection.hpp>
#include <raytracer/Material.hpp>
#include <raytracer/math.hpp>
#include <raytracer/Random.hpp>
#include <raytracer/Ray.hpp>

namespace udit::raytracer
{

    struct Diffuse_Material : public Material
    {
        Color albedo;

        Diffuse_Material(Color given_albedo)
        {
            albedo = given_albedo;
        }

        virtual bool scatter (const Ray & , Ray & scattered_ray, const Intersection & intersection, Color & attenuation)
        {
            auto   target = intersection.point + intersection.normal + random.point_inside_sphere ();

            scattered_ray = Ray{intersection.point, target - intersection.point};
            attenuation   = albedo;

            return true;
        }
    };

}
