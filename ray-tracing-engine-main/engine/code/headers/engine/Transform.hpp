/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <engine/Component.hpp>
#include <engine/math.hpp>

namespace udit::engine
{

    struct Transform : public engine::Component
    {
        Vector3 position;
        Vector3 rotation;
        Vector3 scales;

    public:

        Transform()
        :
            position(0, 0, 0),
            rotation(0, 0, 0),
            scales  (1, 1, 1)
        {
        }

        Transform(const Vector3 & given_position)
        :
            position(given_position),
            rotation(0, 0, 0),
            scales  (1, 1, 1)
        {
        }

        Transform(const Vector3 & given_position, const Vector3 & given_rotation, const Vector3 & given_scales)
        :
            position(given_position),
            rotation(given_rotation),
            scales  (given_scales  )
        {
        }
    };

}
