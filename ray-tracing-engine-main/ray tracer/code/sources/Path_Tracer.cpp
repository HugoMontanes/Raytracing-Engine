/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <iostream>
#include <locale>
#include <chrono>
#include <thread>

#include <raytracer/Intersectable.hpp>
#include <raytracer/Intersection.hpp>
#include <raytracer/Material.hpp>
#include <raytracer/Path_Tracer.hpp>
#include <raytracer/Sky_Environment.hpp>

namespace udit::raytracer
{
    // Thread-local storage for efficient tile processing
    thread_local Path_Tracer::Tile_Buffer Path_Tracer::tile_buffer(128 * 128);

    // ENHANCED: Continuous update control with proper synchronization

    void Path_Tracer::start_continuous_updates(float updates_per_second)
    {
        // Safety check: prevent multiple background threads
        if (continuous_updates_active.load()) {
            return;
        }

        // Configure timing parameters
        snapshot_update_interval = 1.0f / updates_per_second;

        // Initialize synchronization state
        // Starting with tiles_completed = true means we're ready to take the first snapshot
        tiles_completed.store(true);
        active_tile_count.store(0);

        // Activate the continuous update system
        continuous_updates_active.store(true);

        // Start the synchronized background thread
        // This thread will wait for complete tile iterations before taking snapshots
        snapshot_update_thread = std::thread(&Path_Tracer::continuous_snapshot_update_loop_synchronized, this);

        std::cout << "Synchronized continuous updates started at " << updates_per_second << " FPS" << std::endl;
    }

    void Path_Tracer::stop_continuous_updates()
    {
        // Signal shutdown to background thread
        continuous_updates_active.store(false);

        // Wake up the background thread if it's waiting
        // This ensures responsive shutdown even if the thread is sleeping
        {
            std::lock_guard<std::mutex> lock(snapshot_sync_mutex);
            tiles_completed.store(true);
        }
        snapshot_ready_condition.notify_all();

        // Wait for clean thread termination
        if (snapshot_update_thread.joinable()) {
            snapshot_update_thread.join();
        }

        std::cout << "Synchronized continuous updates stopped" << std::endl;
    }

    bool Path_Tracer::is_continuous_updates_active() const
    {
        return continuous_updates_active.load();
    }

    void Path_Tracer::set_update_rate(float updates_per_second)
    {
        // Update timing - the background thread will use this new interval
        snapshot_update_interval = 1.0f / updates_per_second;
    }

    void Path_Tracer::continuous_snapshot_update_loop_synchronized()
    {
        // This is the heart of our synchronized continuous update system
        // It ensures we only take snapshots when we have complete, consistent data

        Timer frame_timer;

        while (continuous_updates_active.load()) {
            frame_timer.reset();

            // CRITICAL SECTION: Wait for complete tile iteration
            // This is like waiting for the orchestra to finish a complete musical phrase
            {
                std::unique_lock<std::mutex> lock(snapshot_sync_mutex);

                // Wait until all tiles have completed their work
                // The lambda function defines our "wait condition" - when should we proceed?
                snapshot_ready_condition.wait(lock, [this] {
                    return tiles_completed.load() || !continuous_updates_active.load();
                    });

                // Check if we're shutting down
                if (!continuous_updates_active.load()) {
                    break;
                }

                // At this point, we KNOW all tiles have finished their current iteration
                // This guarantees that framebuffer and ray_counters contain consistent data
                if (!framebuffer.empty() && !ray_counters.empty() &&
                    framebuffer.size() == ray_counters.size() &&
                    framebuffer.size() == snapshot.size()) {

                    // Perform the normalization with guaranteed data consistency
                    // Each pixel's color and sample count are now perfectly synchronized
                    for (unsigned i = 0, size = framebuffer.size(); i < size; ++i) {
                        if (ray_counters[i] > 0.0f) {
                            // This division now uses perfectly matched numerator and denominator
                            snapshot[i] = framebuffer[i] / ray_counters[i];
                        }
                        // Note: We don't reset pixels to black - this preserves image quality
                        // during brief moments when no new samples are available
                    }
                }

                // Reset the completion flag for the next iteration
                // This tells the system we're ready to start accumulating the next set of samples
                tiles_completed.store(false);
            }

            // Maintain target frame rate with precise timing
            float elapsed_time = frame_timer.get_elapsed<Seconds>();
            if (elapsed_time < snapshot_update_interval) {
                float sleep_time = snapshot_update_interval - elapsed_time;
                auto sleep_microseconds = static_cast<long>(sleep_time * 1000000);
                std::this_thread::sleep_for(std::chrono::microseconds(sleep_microseconds));
            }
        }
    }

    const Buffer<Color>& Path_Tracer::get_snapshot_for_display()
    {
        // Choose the optimal snapshot method based on current operating mode
        if (continuous_updates_active.load()) {
            // When continuous updates are active, the snapshot buffer is being maintained
            // by our synchronized background thread, so we can return it directly
            return snapshot;
        }
        else {
            // When continuous updates are inactive, compute the snapshot on demand
            // This maintains backward compatibility with the original system
            return get_snapshot();
        }
    }

    // ENHANCED: Sample primary rays stage with proper synchronization
    void Path_Tracer::sample_primary_rays_stage(Frame_Data& frame_data)
    {
        if (!use_multithreading_) {
            // Single-threaded execution path
            auto& sky_environment = *frame_data.space.get_scene().get_sky_environment();
            auto& spatial_data_structure = frame_data.space;
            auto number_of_iterations = frame_data.number_of_iterations;

            // Process each pixel sequentially
            for (unsigned index = 0, end = primary_rays.size(); index < end; ++index) {
                for (unsigned iterations = number_of_iterations; iterations > 0; --iterations) {
                    framebuffer[index] += trace_ray(primary_rays[index], spatial_data_structure, sky_environment, 0);
                    ray_counters[index] += 1;
                }
            }

            // Signal completion of this iteration
            // In single-threaded mode, we can immediately signal completion
            {
                std::lock_guard<std::mutex> lock(snapshot_sync_mutex);
                tiles_completed.store(true);
            }
            snapshot_ready_condition.notify_one();

        }
        else if (use_multithreading_) {
            // Multi-threaded execution path with tile-based parallelism
            unsigned width = primary_rays.get_width();
            unsigned height = primary_rays.get_height();
            unsigned total_pixels = width * height;

            // Adaptive tile sizing based on image resolution
            // Larger images get larger tiles to balance workload distribution
            unsigned TILE_SIZE;
            if (total_pixels < 250000)
                TILE_SIZE = 32;
            else if (total_pixels < 1000000)
                TILE_SIZE = 64;
            else
                TILE_SIZE = 128;

            unsigned tiles_x = (width + TILE_SIZE - 1) / TILE_SIZE;
            unsigned tiles_y = (height + TILE_SIZE - 1) / TILE_SIZE;
            unsigned total_tiles = tiles_x * tiles_y;

            // Initialize tile tracking for this iteration
            // This counter helps us know when all tiles have finished
            active_tile_count.store(total_tiles);

            // Batch processing to manage thread pool efficiently
            unsigned available_threads = std::thread::hardware_concurrency();
            unsigned MAX_TASKS_PER_BATCH = available_threads * 4;

            std::vector<std::pair<unsigned, unsigned>> tile_coords;

            // Generate coordinates for all tiles
            for (unsigned ty = 0; ty < tiles_y; ++ty) {
                for (unsigned tx = 0; tx < tiles_x; ++tx) {
                    tile_coords.emplace_back(tx, ty);
                }
            }

            // Process tiles in manageable batches
            for (size_t batch_start = 0; batch_start < tile_coords.size(); batch_start += MAX_TASKS_PER_BATCH) {
                size_t batch_end = std::min(batch_start + MAX_TASKS_PER_BATCH, tile_coords.size());

                // Submit this batch of tile processing tasks
                for (size_t i = batch_start; i < batch_end; ++i) {
                    unsigned tx = tile_coords[i].first;
                    unsigned ty = tile_coords[i].second;

                    unsigned start_x = tx * TILE_SIZE;
                    unsigned start_y = ty * TILE_SIZE;
                    unsigned end_x = std::min(start_x + TILE_SIZE, width);
                    unsigned end_y = std::min(start_y + TILE_SIZE, height);

                    // Use the synchronized tile method to ensure proper completion signaling
                    submit_task_([this, space_ptr = &frame_data.space, start_x, start_y, end_x, end_y,
                        iterations = frame_data.number_of_iterations]() {
                            trace_tile_synchronized(*space_ptr, start_x, start_y, end_x, end_y, iterations);
                        });
                }

                // Wait for this batch to complete before submitting the next batch
                wait_for_tasks_();
            }

            // At this point, all tiles have been submitted and completed
            // The last tile to finish will have already signaled completion
        }
    }

    // ENHANCED: Synchronized tile processing with completion signaling
    void Path_Tracer::trace_tile_synchronized(Spatial_Data_Structure& space,
        unsigned start_x, unsigned start_y,
        unsigned end_x, unsigned end_y,
        unsigned number_of_iterations)
    {
        auto& sky_environment = *space.get_scene().get_sky_environment();

        // Calculate tile dimensions for efficient processing
        unsigned tile_width = end_x - start_x;
        unsigned tile_height = end_y - start_y;
        unsigned tile_size = tile_width * tile_height;

        // Use thread-local storage to avoid memory allocation overhead
        tile_buffer.reset(tile_size);
        uint64_t local_ray_count = 0;

        // Process every pixel in this tile
        for (unsigned y = start_y; y < end_y; ++y) {
            for (unsigned x = start_x; x < end_x; ++x) {
                unsigned buffer_index = y * primary_rays.get_width() + x;
                unsigned tile_index = (y - start_y) * tile_width + (x - start_x);

                // Cast multiple rays per pixel for higher quality
                for (unsigned iterations = number_of_iterations; iterations > 0; --iterations) {
                    tile_buffer.colors[tile_index] += trace_ray(
                        primary_rays[buffer_index], space, sky_environment, 0);
                    tile_buffer.counters[tile_index] += 1;
                }

                local_ray_count += number_of_iterations;
            }
        }

        // Update global performance counters
        benchmark.emitted_ray_count.fetch_add(local_ray_count, std::memory_order_relaxed);

        // CRITICAL SECTION: Update the global framebuffer with this tile's results
        // This section must be atomic to prevent data corruption
        {
            std::lock_guard<std::mutex> lock(framebuffer_mutex);
            for (unsigned y = start_y; y < end_y; ++y) {
                for (unsigned x = start_x; x < end_x; ++x) {
                    unsigned buffer_index = y * primary_rays.get_width() + x;
                    unsigned tile_index = (y - start_y) * tile_width + (x - start_x);

                    // Accumulate this tile's samples into the global buffers
                    framebuffer[buffer_index] += tile_buffer.colors[tile_index];
                    ray_counters[buffer_index] += tile_buffer.counters[tile_index];
                }
            }
        }

        // SYNCHRONIZATION: Check if this was the last tile to complete
        // This is the key mechanism that ensures we only take snapshots of complete data
        int remaining_tiles = active_tile_count.fetch_sub(1) - 1;
        if (remaining_tiles == 0) {
            // This was the last tile - all data is now consistent and complete
            // Signal the snapshot thread that it's safe to take a snapshot
            {
                std::lock_guard<std::mutex> lock(snapshot_sync_mutex);
                tiles_completed.store(true);
            }
            snapshot_ready_condition.notify_one();
        }
    }

    // Original tile method preserved for backward compatibility
    void Path_Tracer::trace_tile(Spatial_Data_Structure& space,
        unsigned start_x, unsigned start_y,
        unsigned end_x, unsigned end_y,
        unsigned number_of_iterations)
    {
        // This method maintains the original behavior for non-continuous rendering
        auto& sky_environment = *space.get_scene().get_sky_environment();

        unsigned tile_width = end_x - start_x;
        unsigned tile_height = end_y - start_y;
        unsigned tile_size = tile_width * tile_height;

        tile_buffer.reset(tile_size);
        uint64_t local_ray_count = 0;

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

        benchmark.emitted_ray_count.fetch_add(local_ray_count, std::memory_order_relaxed);

        // Update framebuffer without synchronization signaling
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

    // Benchmark and performance monitoring methods
    void Path_Tracer::end_benchmark_stage(Frame_Data&)
    {
        benchmark.runtime += benchmark.timer.get_elapsed<Seconds>();

        if (benchmark.runtime > 5.0) {
            std::cout.imbue(std::locale(""));
            std::cout << uint64_t(double(benchmark.emitted_ray_count) / benchmark.runtime) << " rays/s" << std::endl;

            benchmark.runtime = 0.0;
            benchmark.emitted_ray_count = 0;
        }
    }

    // Core ray tracing algorithm - follows a single ray through the scene
    Color Path_Tracer::trace_ray(
        const Ray& ray,
        Spatial_Data_Structure& spatial_data_structure,
        const Sky_Environment& sky_environment,
        unsigned                 depth
    )
    {
        benchmark.emitted_ray_count.fetch_add(1, std::memory_order_relaxed);

        Intersection intersection;

        // Test if the ray hits any objects in the scene
        if (spatial_data_structure.traverse(ray, 0.0001f, 10000.f, intersection)) {
            Ray   scattered_ray;
            Color attenuation;

            // Ask the material how light should scatter from this surface
            if (intersection.intersectable->material->scatter(ray, scattered_ray, intersection, attenuation)) {
                if (depth < recursion_limit) {
                    // Recursively trace the scattered ray to simulate light bouncing
                    return attenuation * trace_ray(scattered_ray, spatial_data_structure, sky_environment, depth + 1);
                }

                return attenuation;
            }

            return Color(0, 0, 0);
        }
        else {
            // Ray didn't hit anything - sample the sky environment
            return sky_environment.sample(glm::normalize(ray.direction));
        }
    }

    // Multithreading control methods
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