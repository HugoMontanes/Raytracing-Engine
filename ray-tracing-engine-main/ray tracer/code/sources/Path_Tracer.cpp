/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <iostream>
#include <locale>

#include <raytracer/Intersectable.hpp>
#include <raytracer/Intersection.hpp>
#include <raytracer/Material.hpp>
#include <raytracer/Path_Tracer.hpp>
#include <raytracer/Sky_Environment.hpp>

namespace udit::raytracer
{

    void Path_Tracer::sample_primary_rays_stage (Frame_Data & frame_data)
    {
        auto & sky_environment        = *frame_data.space.get_scene ().get_sky_environment ();
        auto & spatial_data_structure =  frame_data.space;
        auto   number_of_iterations   =  frame_data.number_of_iterations;

        for (unsigned index = 0, end = primary_rays.size (); index < end; ++index)
        {
            for (unsigned iterations = number_of_iterations; iterations > 0; --iterations)
            {
                framebuffer [index] += trace_ray (primary_rays[index], spatial_data_structure, sky_environment, 0);
                ray_counters[index] += 1;
            }
        }
    }

    void Path_Tracer::end_benchmark_stage (Frame_Data & )
    {
        benchmark.runtime += benchmark.timer.get_elapsed< Seconds > ();

        if (benchmark.runtime > 5.0)
        {
            std::cout.imbue (std::locale (""));
            std::cout << uint64_t(double(benchmark.emitted_ray_count) / benchmark.runtime) << " rays/s" << std::endl;

            benchmark.runtime = 0.0;
            benchmark.emitted_ray_count = 0;
        }
    }

    Color Path_Tracer::trace_ray
    (
        const Ray              & ray,
        Spatial_Data_Structure & spatial_data_structure,
        const Sky_Environment  & sky_environment,
        unsigned                 depth
    )
    {
        benchmark.emitted_ray_count++;

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
