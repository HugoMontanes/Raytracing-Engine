/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <functional>
#include <raytracer/Camera.hpp>

namespace udit::raytracer
{

    class Pinhole_Camera : public Camera
    {
    private:

        bool use_multithreading_ = false;
        std::function<void(std::function<void()>)> submit_task_;
        std::function<void()> wait_for_tasks_;

    public:

        Pinhole_Camera(const Sensor_Type & given_sensor_type, float given_focal_length)
        :
            Camera(given_sensor_type, given_focal_length)
        {
        }

        void calculate (Buffer< Ray > & primary_rays) override;

        void enable_multithreading(
            const std::function<void(std::function<void()>)>& submit_task,
            const std::function<void()>& wait_for_tasks
        );

        void disable_multithreading();

    private:

        //Helper method for tile-based ray generation
        void generate_rays_for_tile(
            Buffer< Ray >& primary_rays,
            unsigned start_x, unsigned start_y,
            unsigned end_x, unsigned end_y,
            const Vector3& sensor_bottom_left,
            const Vector3& horizontal_step,
            const Vector3& vertical_step,
            const Vector3& focal_point);
        
    };

}
