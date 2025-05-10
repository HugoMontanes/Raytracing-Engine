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

    void Path_Tracer::trace_tile(Spatial_Data_Structure& space, unsigned start_x, unsigned start_y, unsigned end_x, unsigned end_y, unsigned number_of_iterations)
    {
        // Get the sky environment and spatial acceleration structure
        auto& sky_environment = *space.get_scene().get_sky_environment();

        // Generate primary rays for this tile
        Buffer<Ray> tile_rays;
        tile_rays.resize(end_x - start_x, end_y - start_y);

        // Get camera
        auto camera = space.get_scene().get_camera();
        if (!camera) return;

        // Calculate rays for this tile
        calculate_tile_rays(camera, tile_rays, start_x, start_y, end_x, end_y);

        // Process each pixel in the tile
        for (unsigned y = 0; y < end_y - start_y; ++y) {
            for (unsigned x = 0; x < end_x - start_x; ++x) {
                unsigned local_index = y * (end_x - start_x) + x;
                unsigned global_index = (start_y + y) * framebuffer.get_width() + (start_x + x);

                // For each sample
                for (unsigned iterations = number_of_iterations; iterations > 0; --iterations) {
                    Color sample = trace_ray(tile_rays[local_index], space, sky_environment, 0);

                    // Update the framebuffer with a mutex lock
                    std::lock_guard<std::mutex> lock(framebuffer_mutex);
                    framebuffer[global_index] += sample;
                    ray_counters[global_index] += 1;
                }
            }
        }
    }

    void Path_Tracer::calculate_tile_rays(Camera* camera, Buffer<Ray>& rays, unsigned start_x, unsigned start_y, unsigned end_x, unsigned end_y)
    {
        //Get sensor dimensions
        float sensor_width = camera->get_sensor_width();
        float aspect_ratio = static_cast<float>(framebuffer.get_width()) / framebuffer.get_height();
        float sensor_height = sensor_width / aspect_ratio;

        //Calculate camera parameters
        float half_width = sensor_width * 0.5f;
        float half_height = sensor_height * 0.5f;
        float z = -camera->get_focal_length();

        //Calculate rays for the tile
        for (unsigned y = 0; y < rays.get_height(); ++y) 
        {
            for (unsigned x = 0; x < rays.get_width(); ++x) 
            {
                // Convert to normalized device coordinates including the offset
                float ndc_x = (static_cast<float>(start_x + x) + 0.5f) / framebuffer.get_width();
                float ndc_y = (static_cast<float>(start_y + y) + 0.5f) / framebuffer.get_height();

                // Convert to sensor coordinates
                float sensor_x = (2.0f * ndc_x - 1.0f) * half_width;
                float sensor_y = (1.0f - 2.0f * ndc_y) * half_height;

                // Create ray in camera space
                Vector3 ray_origin(0, 0, 0);
                Vector3 ray_direction = normalize(Vector3(sensor_x, sensor_y, z));

                // Transform ray to world space
                Matrix4 view_matrix = camera->transform.get_matrix();
                Ray ray;
                ray.origin = Vector3(view_matrix * Vector4(ray_origin, 1.0f));
                ray.direction = normalize(Vector3(view_matrix * Vector4(ray_direction, 0.0f)));

                // Store the ray
                rays[y * rays.get_width() + x] = ray;
            }
        }
    }

}
