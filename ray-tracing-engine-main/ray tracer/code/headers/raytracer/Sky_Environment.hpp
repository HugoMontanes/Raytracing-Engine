/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/Color.hpp>

namespace udit::raytracer
{

    class Sky_Environment
    {
    public:

        virtual Color sample (const Vector3 & direction) const = 0;

        virtual ~Sky_Environment() = default;
    };

}
