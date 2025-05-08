/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <atomic>
#include <cstdint>

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

        struct Frame_Data
        {
            Spatial_Data_Structure & space;
            const unsigned  viewport_width;
            const unsigned  viewport_height;
            const unsigned  number_of_iterations;
        };

    private:

        static constexpr unsigned recursion_limit = 10;

        Buffer< Color > framebuffer;
        Buffer< float > ray_counters;
        Buffer< Ray   > primary_rays;
        Buffer< Color > snapshot;

        struct
        {
            using Counter = std::atomic< uint64_t >;

            Timer   timer;
            double  runtime = 0.0;
            Counter emitted_ray_count = 0;
        }
        benchmark;

    public:

        Path_Tracer()
        {
            ray_counters.clear (0.f);
        }

        const Buffer< Color > & get_frame_buffer () const
        {
            return framebuffer;
        }

        const Buffer< Color > & get_snapshot ()
        {
            for (unsigned i = 0, size = framebuffer.size (); i < size; ++i)
            {
                snapshot[i] = framebuffer[i] / ray_counters[i];
            }

            return snapshot;
        }

    public:

        void trace
        (
            Spatial_Data_Structure & space,
            unsigned viewport_width,
            unsigned viewport_height,
            unsigned number_of_iterations
        )
        {
            Frame_Data frame_data{ space, viewport_width, viewport_height, number_of_iterations };

            execute_path_tracing_pipeline (frame_data);
        }

    private:

        void execute_path_tracing_pipeline (Frame_Data & frame_data)
        {
            start_benchmark_stage     (frame_data);
            prepare_buffers_stage     (frame_data);
            check_camera_change_stage (frame_data);
            build_primary_rays_stage  (frame_data);
            prepare_space_stage       (frame_data);
            sample_primary_rays_stage (frame_data);
            end_benchmark_stage       (frame_data);
        }

        void start_benchmark_stage (Frame_Data & )
        {
            benchmark.timer.reset ();
        };

        void prepare_buffers_stage (Frame_Data & frame_data)
        {
            framebuffer .resize (frame_data.viewport_width, frame_data.viewport_height);
            primary_rays.resize (frame_data.viewport_width, frame_data.viewport_height);
            ray_counters.resize (frame_data.viewport_width, frame_data.viewport_height);
            snapshot    .resize (frame_data.viewport_width, frame_data.viewport_height);
        }

        void check_camera_change_stage (Frame_Data & frame_data)
        {
            auto camera = frame_data.space.get_scene ().get_camera ();

            if (camera->transform.has_changed (true))
            {
                 framebuffer.clear (Color(0, 0, 0));
                ray_counters.clear (0.f);
            }
        }

        void build_primary_rays_stage (Frame_Data & frame_data)
        {
            auto camera = frame_data.space.get_scene ().get_camera ();

            assert(camera != nullptr);

            camera->calculate (primary_rays);
        }

        void prepare_space_stage (Frame_Data & frame_data)
        {
            if (not frame_data.space.is_ready ())
            {
                frame_data.space.classify_intersectables ();
            }
        }

        void sample_primary_rays_stage (Frame_Data & frame_data);

        void end_benchmark_stage (Frame_Data & frame_data);

    private:

        Color trace_ray
        (
            const Ray              & ray,
            Spatial_Data_Structure & spatial_data_structure,
            const Sky_Environment  & sky_environment,
            unsigned                 depth
        );

    };

}
