/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <raytracer/Pinhole_Camera.hpp>

#include <iostream>
using namespace std;

namespace udit::raytracer
{

    void Pinhole_Camera::calculate (Buffer< Ray > & primary_rays)
    {
        auto buffer_width  = primary_rays.get_width  ();
        auto buffer_height = primary_rays.get_height ();

        Vector2 half_sensor_resolution
        (
            0.5f * static_cast< float >(buffer_width ),
            0.5f * static_cast< float >(buffer_height)
        );

        Vector2 half_sensor_size
        (
            0.5f * get_sensor_width (),
            0.5f * get_sensor_width () * half_sensor_resolution.y / half_sensor_resolution.x
        );

        auto  & transform_matrix   = transform.get_matrix   ();
        Vector3 sensor_center      = transform.get_position ();
        Vector3 focal_point        = transform_matrix * Vector4(0, 0, -focal_length, 1);
        Vector3 right_direction    = transform_matrix * Vector4(half_sensor_size.x, 0, 0, 0);
        Vector3 up_direction       = transform_matrix * Vector4(0, half_sensor_size.y, 0, 0);
        Vector3 sensor_bottom_left = sensor_center - (right_direction + up_direction);

        Vector3 horizontal_step    = right_direction / half_sensor_resolution.x;
        Vector3 vertical_step      = up_direction    / half_sensor_resolution.y;

        Vector3 scanline_start     = sensor_bottom_left;
        auto    buffer_offset      = buffer_width * buffer_height;

        for (auto row = buffer_height; row > 0; --row, scanline_start += vertical_step)
        {
            Vector3 pixel = scanline_start;

            for (auto column = buffer_width; column > 0; --column, pixel += horizontal_step)
            {
                primary_rays[--buffer_offset] = Ray{ pixel, focal_point - pixel };
            }
        }
    }

}
