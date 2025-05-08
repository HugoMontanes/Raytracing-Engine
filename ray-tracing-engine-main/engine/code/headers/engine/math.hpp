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

    #include <engine/dependencies/glm/glm.hpp>
    #include <engine/dependencies/glm/gtc/quaternion.hpp>

#endif

namespace udit::engine
{

    // Vectores:

    template< unsigned DIMENSION, typename TYPE >
    using Vector  = glm::vec< DIMENSION, TYPE >;

    using Vector2 = Vector< 2, float >;
    using Vector3 = Vector< 3, float >;
    using Vector4 = Vector< 4, float >;

    // Matrices:

    template< unsigned COLUMNS, unsigned ROWS, typename TYPE >
    using Matrix  = glm::mat< static_cast< glm::length_t >(COLUMNS), static_cast< glm::length_t >(ROWS), TYPE >;

    using Matrix2 = Matrix< 2, 2, float >;
    using Matrix3 = Matrix< 2, 2, float >;
    using Matrix4 = Matrix< 2, 2, float >;

    // Cuaternión:

    template< typename VALUE_TYPE = float >
    using Quaternion = glm::qua< VALUE_TYPE >;

}
