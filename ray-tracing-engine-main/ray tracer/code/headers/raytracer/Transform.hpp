/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <raytracer/math.hpp>

namespace udit::raytracer
{

    class Transform
    {
        Vector3  position;
        Vector3  rotation;
        Vector3  scales;

        Matrix4  matrix;
        unsigned cached;
        unsigned changed;

    public:

        Transform()
        {
            position = Vector3(0);
            rotation = Vector3(0);
            scales   = Vector3(1);
            cached   = false;
            changed  = false;
        }

        bool has_changed (bool reset)
        {
            auto has_changed = changed;

            if (reset) changed = false;

            return has_changed;
        }

        const Vector3 & get_position () const { return position; }
        const Vector3 & get_rotation () const { return rotation; }
        const Vector3 & get_scales   () const { return scales;   }

        void set_position (const Vector3 & new_position)
        {
            if (position != new_position)
            {
                position  = new_position;
                cached    = false;
            }
        }

        void set_rotation (const Vector3 & new_rotation)
        {
            if (rotation != new_rotation)
            {
                rotation  = new_rotation;
                cached    = false;
            }
        }

        void set_scales (const Vector3 & new_scales)
        {
            if (scales != new_scales)
            {
                scales  = new_scales;
                cached = false;
            }
        }

        const Matrix4 & get_matrix ()
        {
            if (not cached)
            {
                matrix  = Matrix4(1);
                matrix  = translate (matrix, position);
              //matrix  = rotate    (matrix, rotation.y, Vector3(0, 1, 0));
              //matrix  = scale     (matrix, scales  );
                cached  = true;
                changed = true;
            }

            return matrix;
        }

    };

}
