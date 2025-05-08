/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <raytracer/Camera.hpp>

namespace udit::raytracer
{

    const float Camera::sensor_widths[] =
    {
        0.0360f,                            // FULL_FRAME
        0.0236f                             // APS_C
    };

}
