/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/Id.hpp>
#include <raytracer/Transform.hpp>

namespace udit::raytracer
{

    struct Node
    {
        Id id;

        Transform transform;

        virtual ~Node() = default;

        virtual void apply_transform () { }
    };

}
