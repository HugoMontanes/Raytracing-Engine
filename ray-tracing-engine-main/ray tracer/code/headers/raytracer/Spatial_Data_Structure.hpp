/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/declarations.hpp>

namespace udit::raytracer
{

    class Spatial_Data_Structure
    {
    protected:

        Scene & scene;
        bool    ready;

    public:

        Spatial_Data_Structure(Scene & given_scene) : scene(given_scene)
        {
            ready = false;
        }

        virtual ~Spatial_Data_Structure() = default;

    public:

        Scene & get_scene () const
        {
            return scene;
        }

        bool is_ready () const
        {
            return ready;
        }

    public:

        virtual void classify_intersectables () = 0;

        virtual bool traverse (const Ray & ray, float min_t, float max_t, Intersection & intersection) const = 0;

    };

}
