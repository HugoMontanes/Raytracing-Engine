/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <memory>
#include <vector>

#include <raytracer/declarations.hpp>
#include <raytracer/Node.hpp>

namespace udit::raytracer
{

    struct Model : public Node
    {
        using Intersectable_List = std::vector< Intersectable * >;

        Intersectable_List intersectables;

        void add (Intersectable * intersectable)
        {
            intersectables.push_back (intersectable);
        }
    };

}
