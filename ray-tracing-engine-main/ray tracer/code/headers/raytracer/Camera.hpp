/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/Buffer.hpp>
#include <raytracer/Node.hpp>
#include <raytracer/Ray.hpp>

namespace udit::raytracer
{

    class Camera : public Node
    {
    public:

        enum Sensor_Type
        {
            FULL_FRAME,
            APS_C,
        };

        static const float sensor_widths[];

    protected:

        Sensor_Type sensor_type;
        float       focal_length;

    public:

        Camera(const Sensor_Type & given_sensor_type, float given_focal_length)
        {
            sensor_type  = given_sensor_type;
            focal_length = given_focal_length;
        }

        virtual ~Camera() = default;

    public:

        Sensor_Type get_sensor_type () const
        {
            return sensor_type;
        }

        float get_focal_length () const
        {
            return focal_length;
        }

        void set_focal_length (float new_focal_length)
        {
            focal_length = new_focal_length;
        }

        float get_sensor_width () const
        {
            return sensor_widths[sensor_type];
        }

    public:

        virtual void calculate (Buffer< Ray > & primary_rays) = 0;

    };

}
