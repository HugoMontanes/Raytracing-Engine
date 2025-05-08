/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <cmath>

#ifndef GLM_INCLUDED
#define GLM_INCLUDED

    #include <raytracer/dependencies/glm/glm.hpp>
    #include <raytracer/dependencies/glm/gtc/quaternion.hpp>

#endif

namespace udit::raytracer
{

    // VECTOR:

    template< unsigned DIMENSION, typename TYPE >
    using Vector  = glm::vec< DIMENSION, TYPE >;

    using Vector2 = Vector< 2, float >;
    using Vector3 = Vector< 3, float >;
    using Vector4 = Vector< 4, float >;

    // MATRIX:

    template< unsigned COLUMNS, unsigned ROWS, typename TYPE >
    using Matrix  = glm::mat< static_cast< glm::length_t >(COLUMNS), static_cast< glm::length_t >(ROWS), TYPE >;

    using Matrix2 = Matrix< 2, 2, float >;
    using Matrix3 = Matrix< 3, 3, float >;
    using Matrix4 = Matrix< 4, 4, float >;

    // QUATERNION:

    template< typename VALUE_TYPE = float >
    using Quaternion = glm::qua< VALUE_TYPE >;

    // MIX:

    template< typename VALUE_TYPE, typename FACTOR_TYPE >
    constexpr inline VALUE_TYPE mix (const VALUE_TYPE & a, const VALUE_TYPE & b, const FACTOR_TYPE & t)
    {
        return glm::mix (a, b, t);
    }

    // NORMALIZE:

    template< unsigned D, typename T >
    constexpr inline Vector< D, T > normalize (const Vector< D, T > & vector)
    {
        return glm::normalize (vector);
    }

    // DOT:

    template< unsigned D, typename T >
    constexpr inline T dot (const Vector< D, T > & a, const Vector< D, T > & b)
    {
        return glm::dot (a, b);
    }

    // REFLECT:

    template< unsigned D, typename T >
    constexpr inline Vector< D, T > reflect (const Vector< D, T > & a, const Vector< D, T > & b)
    {
        return glm::reflect (a, b);
    }

    // TRANSLATE:

    template< unsigned D, typename T >
    constexpr inline Matrix< D, D, T > translate (const Matrix< D, D, T > & matrix, const Vector< D, T > & translation)
    {
        return glm::translate (matrix, translation);
    }

    // ROTATE:

    template< unsigned D, typename T >
    constexpr inline Matrix< D, D, T > rotate (const Matrix< D, D, T > & matrix, const T & angle, const Vector< D, T > & axis)
    {
        return glm::rotate (matrix, angle, axis);
    }

    // SCALE:

    template< unsigned D, typename T >
    constexpr inline Matrix< D, D, T > scale (const Matrix< D, D, T > & matrix, const Vector< D, T > & scale)
    {
        return glm::scale (matrix, scale);
    }

    // SCALE:

    template< typename T >
    constexpr inline Matrix< 4, 4, T > euler_angles_to_quaternion_to_matrix (const Vector< 3, T > & euler_angles)
    {
        return glm::mat4_cast (Quaternion< T >(euler_angles));
    }

}
