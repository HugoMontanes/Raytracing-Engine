/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <raytracer/Intersectable.hpp>
#include <raytracer/Intersection.hpp>
#include <raytracer/Linear_Space.hpp>
#include <raytracer/Model.hpp>
#include <raytracer/Ray.hpp>

namespace udit::raytracer
{

    void Linear_Space::classify_intersectables ()
    {
        size_t number_of_intersectables = 0;

        for (auto & model : scene)
        {
            number_of_intersectables += model.intersectables.size ();
        }

        intersectables.clear   ();
        intersectables.reserve (number_of_intersectables);

        for (auto & model : scene)
        {
            for (auto & intersectable : model.intersectables)
            {
                intersectables.push_back (intersectable);
            }
        }

        ready = true;
    }

    bool Linear_Space::traverse (const Ray & ray, float min_t, float max_t, Intersection & closest_intersection) const
    {
        closest_intersection.t = max_t;

        for (auto & intersectable : intersectables)
        {
            float t = intersectable->intersect (ray, min_t, closest_intersection.t);

            if (t > 0.f)
            {
                closest_intersection.t = t;
                closest_intersection.intersectable = intersectable;
            }
        }

        if (closest_intersection.t < max_t)
        {
            closest_intersection.point  = ray.point_at (closest_intersection.t);
            closest_intersection.normal = closest_intersection.intersectable->normal_at (closest_intersection.point);

            return true;
        }

        return false;
    }

}
