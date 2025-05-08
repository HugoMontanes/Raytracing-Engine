/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <vector>
#include <raytracer/Scene.hpp>
#include <raytracer/Spatial_Data_Structure.hpp>

namespace udit::raytracer
{

    class Linear_Space : public Spatial_Data_Structure
    {
        using Intersectable_List = std::vector< Intersectable * >;

    private:

        Intersectable_List intersectables;

    public:

        Linear_Space(Scene & given_scene) : Spatial_Data_Structure(given_scene)
        {
        }

    public:

        void classify_intersectables () override;

        bool traverse (const Ray & ray, float min_t, float max_t, Intersection & intersection) const override;

    };

}
