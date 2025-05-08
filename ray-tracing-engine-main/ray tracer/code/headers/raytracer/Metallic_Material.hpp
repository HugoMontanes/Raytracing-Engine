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

    struct Metallic_Material : public Material
    {
        static constexpr float epsilon = 0.001f;

        Color albedo;
        float diffusion;

        Metallic_Material(Color given_albedo, float given_diffusion)
        {
            albedo    = given_albedo;
            diffusion = given_diffusion < 1.f ? given_diffusion : 1.f;
        }

        virtual bool scatter (const Ray & incident_ray, Ray & scattered_ray, const Intersection & intersection, Color & attenuation)
        {
            Vector3  reflected_direction = reflect (normalize (incident_ray.direction), intersection.normal);

            if (dot (reflected_direction, intersection.normal) > 0.f)
            {
                if (diffusion < epsilon)
                {
                    scattered_ray = Ray{intersection.point, reflected_direction};
                }
                else
                {
                    scattered_ray = Ray{intersection.point, reflected_direction + diffusion * 0.5f * random.point_inside_sphere ()};
                }

                attenuation = albedo;

                return true;
            }

            return false;
        }
    };

}
