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
    thread_local Path_Tracer::Tile_Buffer Path_Tracer::tile_buffer(128 * 128);


    void Path_Tracer::sample_primary_rays_stage (Frame_Data & frame_data)
    {
        if (!use_multithreading_)
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
        else if(use_multithreading_)
        {

            // Calculate number of tiles in each dimension
            unsigned width = primary_rays.get_width();
            unsigned height = primary_rays.get_height();

            unsigned total_pixels = width * height;
            unsigned covered_pixels = 0;

            unsigned TILE_SIZE;
            if (total_pixels < 250000) // 500x500 or smaller
                TILE_SIZE = 32;
            else if (total_pixels < 1000000) // 1000x1000 or smaller
                TILE_SIZE = 64;
            else
                TILE_SIZE = 128;

            unsigned tiles_x = (width + TILE_SIZE - 1) / TILE_SIZE;
            unsigned tiles_y = (height + TILE_SIZE - 1) / TILE_SIZE;
            unsigned total_tiles = tiles_x * tiles_y;

            // Get number of hardware threads for adaptive task scheduling
            unsigned available_threads = std::thread::hardware_concurrency();
            unsigned MAX_TASKS_PER_BATCH = available_threads * 4; // Queue depth

            // Process tiles in batches
            std::vector<std::pair<unsigned, unsigned>> tile_coords;

            // Create and submit tasks for each tile
            for (unsigned ty = 0; ty < tiles_y; ++ty) {
                for (unsigned tx = 0; tx < tiles_x; ++tx) {
                    tile_coords.emplace_back(tx, ty);

                }
            }
            for (size_t batch_start = 0; batch_start < tile_coords.size(); batch_start += MAX_TASKS_PER_BATCH) {
                size_t batch_end = std::min(batch_start + MAX_TASKS_PER_BATCH, tile_coords.size());

                // Submit this batch of tasks
                for (size_t i = batch_start; i < batch_end; ++i) {
                    unsigned tx = tile_coords[i].first;
                    unsigned ty = tile_coords[i].second;

                    // Calculate tile bounds
                    unsigned start_x = tx * TILE_SIZE;
                    unsigned start_y = ty * TILE_SIZE;
                    unsigned end_x = std::min(start_x + TILE_SIZE, width);
                    unsigned end_y = std::min(start_y + TILE_SIZE, height);

                    // Submit task for this tile
                    submit_task_([this, space_ptr = &frame_data.space, start_x, start_y, end_x, end_y,
                        iterations = frame_data.number_of_iterations]() {
                            trace_tile(
                                *space_ptr,
                                start_x, start_y,
                                end_x, end_y,
                                iterations); });
                }

                // Wait for all tasks in this batch to complete
                wait_for_tasks_();
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
        benchmark.emitted_ray_count.fetch_add(1, std::memory_order_relaxed);

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
            return sky_environment.sample (glm::normalize (ray.direction));
    }

    void Path_Tracer::trace_tile(Spatial_Data_Structure& space,
        unsigned start_x, unsigned start_y,
        unsigned end_x, unsigned end_y,
        unsigned number_of_iterations) {
        auto start_time = std::chrono::high_resolution_clock::now();

        auto& sky_environment = *space.get_scene().get_sky_environment();

        // Compute tile dimensions
        unsigned tile_width = end_x - start_x;
        unsigned tile_height = end_y - start_y;
        unsigned tile_size = tile_width * tile_height;

        // Use thread-local storage to avoid allocation
        tile_buffer.reset(tile_size);

        // Track ray count for benchmarking
        uint64_t local_ray_count = 0;

        // Process all pixels in the tile without locking
        for (unsigned y = start_y; y < end_y; ++y) {
            for (unsigned x = start_x; x < end_x; ++x) {
                unsigned buffer_index = y * primary_rays.get_width() + x;
                unsigned tile_index = (y - start_y) * tile_width + (x - start_x);

                for (unsigned iterations = number_of_iterations; iterations > 0; --iterations) {
                    tile_buffer.colors[tile_index] += trace_ray(
                        primary_rays[buffer_index], space, sky_environment, 0);
                    tile_buffer.counters[tile_index] += 1;
                }

                local_ray_count += number_of_iterations;
            }
        }

        // Update benchmark counter once per tile 
        benchmark.emitted_ray_count.fetch_add(local_ray_count, std::memory_order_relaxed);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time).count();

        // Lock once to update the framebuffer with all the tile's results
        std::lock_guard<std::mutex> lock(framebuffer_mutex);
        for (unsigned y = start_y; y < end_y; ++y) {
            for (unsigned x = start_x; x < end_x; ++x) {
                unsigned buffer_index = y * primary_rays.get_width() + x;
                unsigned tile_index = (y - start_y) * tile_width + (x - start_x);

                framebuffer[buffer_index] += tile_buffer.colors[tile_index];
                ray_counters[buffer_index] += tile_buffer.counters[tile_index];
            }
        }
    }

    void Path_Tracer::enable_multithreading(const std::function<void(std::function<void()>)>& submit_task, const std::function<void()>& wait_for_tasks)
    {
        use_multithreading_ = true;
        submit_task_ = submit_task;
        wait_for_tasks_ = wait_for_tasks;
    }

    void Path_Tracer::disable_multithreading()
    {
        use_multithreading_ = false;
        submit_task_ = nullptr;
        wait_for_tasks_ = nullptr;
    }


}
