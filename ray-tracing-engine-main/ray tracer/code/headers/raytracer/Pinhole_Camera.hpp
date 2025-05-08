/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/Camera.hpp>

namespace udit::raytracer
{

    class Pinhole_Camera : public Camera
    {
    public:

        Pinhole_Camera(const Sensor_Type & given_sensor_type, float given_focal_length)
        :
            Camera(given_sensor_type, given_focal_length)
        {
        }

        void calculate (Buffer< Ray > & primary_rays) override;

    };

}
