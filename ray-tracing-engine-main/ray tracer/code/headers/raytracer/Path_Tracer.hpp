/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>
#include <functional>

#include <raytracer/Buffer.hpp>
#include <raytracer/Camera.hpp>
#include <raytracer/Color.hpp>
#include <raytracer/Scene.hpp>
#include <raytracer/Spatial_Data_Structure.hpp>
#include <raytracer/Timer.hpp>

namespace udit::raytracer
{

    class Path_Tracer
    {
        // This structure encapsulates all the information needed to render one complete frame
        // Think of it as a "work order" that contains everything the rendering pipeline needs
        struct Frame_Data
        {
            Spatial_Data_Structure& space;
            const unsigned  viewport_width;
            const unsigned  viewport_height;
            const unsigned  number_of_iterations;
        };

    private:

        static constexpr unsigned recursion_limit = 10;

        // Core rendering buffers - these store the accumulated ray tracing results
        Buffer< Color > framebuffer;    // Accumulates color values from all ray samples
        Buffer< float > ray_counters;   // Counts how many ray samples each pixel has received
        Buffer< Ray   > primary_rays;   // The initial rays cast from the camera through each pixel
        Buffer< Color > snapshot;       // The final, normalized image ready for display

        // Performance monitoring system
        struct
        {
            using Counter = std::atomic< uint64_t >;

            Timer   timer;
            double  runtime = 0.0;
            Counter emitted_ray_count = 0;
        }
        benchmark;

        // ENHANCED: Synchronized continuous update system
        // These components work together to ensure we only take snapshots of complete, consistent data
        std::atomic<bool> continuous_updates_active;  // Controls the background update thread
        std::thread snapshot_update_thread;           // Background thread for snapshot updates
        float snapshot_update_interval;               // Target time between snapshots

        // Synchronization primitives - these are the "conductor" that coordinates timing
        std::mutex snapshot_sync_mutex;               // Protects access to synchronization state
        std::condition_variable snapshot_ready_condition;  // Signals when data is ready for snapshot
        std::atomic<bool> tiles_completed;            // Flag indicating all tiles finished their work
        std::atomic<int> active_tile_count;           // Tracks how many tiles are still working

        // The enhanced background method that waits for complete tile iterations
        // This is like waiting for the orchestra to finish a complete phrase before photographing
        void continuous_snapshot_update_loop_synchronized();

    public:

        Path_Tracer()
            :
            continuous_updates_active(false),
            snapshot_update_interval(1.0f / 30.0f),
            tiles_completed(true),     // Start in completed state
            active_tile_count(0)       // No tiles active initially
        {
            ray_counters.clear(0.f);
        }

        // Destructor ensures clean shutdown of all background threads
        ~Path_Tracer()
        {
            stop_continuous_updates();
        }

        // Original interface methods - preserved for backward compatibility
        const Buffer< Color >& get_frame_buffer() const
        {
            return framebuffer;
        }

        // Original snapshot method - computes display buffer on demand
        // This method performs the crucial normalization step that converts raw ray data into displayable colors
        const Buffer< Color >& get_snapshot()
        {
            for (unsigned i = 0, size = framebuffer.size(); i < size; ++i)
            {
                // Normalize accumulated color by the number of samples to get the average
                // This is where the magic happens - converting statistical data into final colors
                if (ray_counters[i] > 0.0f) {
                    snapshot[i] = framebuffer[i] / ray_counters[i];
                }
                else {
                    // If no samples received yet, use black
                    snapshot[i] = Color(0, 0, 0);
                }
            }

            return snapshot;
        }

        // ENHANCED: Optimized snapshot method for continuous display updates
        // This method chooses the best approach based on whether continuous updates are active
        const Buffer< Color >& get_snapshot_for_display();

    public:

        // Main ray tracing entry point - renders the scene with specified quality settings
        void trace
        (
            Spatial_Data_Structure& space,
            unsigned viewport_width,
            unsigned viewport_height,
            unsigned number_of_iterations
        )
        {
            Frame_Data frame_data{ space, viewport_width, viewport_height, number_of_iterations };

            execute_path_tracing_pipeline(frame_data);
        }

        // ENHANCED: Continuous update control methods with proper synchronization
        void start_continuous_updates(float updates_per_second = 30.0f);
        void stop_continuous_updates();
        bool is_continuous_updates_active() const;
        void set_update_rate(float updates_per_second);

    private:

        // The main rendering pipeline - orchestrates all ray tracing operations in sequence
        void execute_path_tracing_pipeline(Frame_Data& frame_data)
        {
            start_benchmark_stage(frame_data);
            prepare_buffers_stage(frame_data);
            check_camera_change_stage(frame_data);
            build_primary_rays_stage(frame_data);
            prepare_space_stage(frame_data);
            sample_primary_rays_stage(frame_data);
            end_benchmark_stage(frame_data);
        }

        // Individual pipeline stages - each has a specific responsibility in the rendering process
        void start_benchmark_stage(Frame_Data&)
        {
            benchmark.timer.reset();
        };

        void prepare_buffers_stage(Frame_Data& frame_data)
        {
            // Ensure all buffers are the correct size for the current viewport
            framebuffer.resize(frame_data.viewport_width, frame_data.viewport_height);
            primary_rays.resize(frame_data.viewport_width, frame_data.viewport_height);
            ray_counters.resize(frame_data.viewport_width, frame_data.viewport_height);
            snapshot.resize(frame_data.viewport_width, frame_data.viewport_height);
        }

        void check_camera_change_stage(Frame_Data& frame_data)
        {
            auto camera = frame_data.space.get_scene().get_camera();

            // If the camera moved, we need to start accumulating samples fresh
            if (camera->transform.has_changed(true))
            {
                framebuffer.clear(Color(0, 0, 0));
                ray_counters.clear(0.f);
            }
        }

        void build_primary_rays_stage(Frame_Data& frame_data)
        {
            auto camera = frame_data.space.get_scene().get_camera();

            assert(camera != nullptr);

            // Generate the initial rays that will be cast through each pixel
            camera->calculate(primary_rays);
        }

        void prepare_space_stage(Frame_Data& frame_data)
        {
            // Ensure the spatial data structure is ready for ray intersection queries
            if (not frame_data.space.is_ready())
            {
                frame_data.space.classify_intersectables();
            }
        }

        // ENHANCED: This method now includes proper synchronization for consistent snapshots
        void sample_primary_rays_stage(Frame_Data& frame_data);
        void end_benchmark_stage(Frame_Data& frame_data);

    private:

        // Core ray tracing algorithm - follows a single ray through the scene
        Color trace_ray
        (
            const Ray& ray,
            Spatial_Data_Structure& spatial_data_structure,
            const Sky_Environment& sky_environment,
            unsigned                 depth
        );

        // Thread synchronization for framebuffer updates
        std::mutex framebuffer_mutex;

    public:

        // ENHANCED: Synchronized tile rendering for consistent data updates
        void trace_tile_synchronized(
            Spatial_Data_Structure& space,
            unsigned start_x, unsigned start_y,
            unsigned end_x, unsigned end_y,
            unsigned number_of_iterations
        );

        // Original tile method preserved for compatibility
        void trace_tile(
            Spatial_Data_Structure& space,
            unsigned start_x, unsigned start_y,
            unsigned end_x, unsigned end_y,
            unsigned number_of_iterations
        );

        // Multithreading control interface
        void enable_multithreading(
            const std::function<void(std::function<void()>)>& submit_task,
            const std::function<void()>& wait_for_tasks
        );

        void disable_multithreading();

    private:
        // Multithreading state management
        bool use_multithreading_ = false;
        std::function<void(std::function<void()>)> submit_task_;
        std::function<void()> wait_for_tasks_;

        // Thread-local tile buffer for efficient parallel processing
        // This avoids memory allocation overhead during rendering
        class Tile_Buffer
        {
        public:
            std::vector<Color> colors;
            std::vector<float> counters;

            Tile_Buffer(size_t max_size)
            {
                colors.resize(max_size);
                counters.resize(max_size);
            }

            void reset(size_t size)
            {
                std::fill_n(colors.begin(), size, Color(0, 0, 0));
                std::fill_n(counters.begin(), size, 0.0f);
            }
        };

        static thread_local Tile_Buffer tile_buffer;
    };
}