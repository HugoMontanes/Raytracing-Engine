/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 */

#include <raytracer/Intersectable.hpp>
#include <raytracer/Intersection.hpp>
#include <raytracer/Material.hpp>
#include <raytracer/Path_Tracer.hpp>
#include <raytracer/Sky_Environment.hpp>

namespace udit::raytracer
{

    Color Path_Tracer::trace_ray
    (
        const Ray              & ray, 
        Spatial_Data_Structure & spatial_data_structure, 
        const Sky_Environment  & sky_environment,
        unsigned                 depth
    )
    {
        Intersection intersection;

        // hacer que min_t sea >= 1 para los rayos primarios...

        if (spatial_data_structure.traverse (ray, 0.0001f, 10000.f, intersection))
        {
            Ray   scattered_ray;
            Color attenuation;

            if (intersection.intersectable->material->scatter (ray, scattered_ray, intersection, attenuation))
            {
                if (depth < recursion_limit)
                {
                    return attenuation * trace_ray (scattered_ray, spatial_data_structure, sky_environment, depth + 1);
                }

                return attenuation;
            }

            return Color(0, 0, 0);
        }
        else
            return sky_environment.sample (normalize (ray.direction));
    }

}
