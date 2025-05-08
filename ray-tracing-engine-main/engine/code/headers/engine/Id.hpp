/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <engine/fnv.hpp>

#define ID(X) FNV(X)

namespace udit::engine
{

    using Id = unsigned int;

    constexpr Id INVALID_ID = ~0u;

    constexpr inline bool not_valid (Id id)
    {
        return id == INVALID_ID;
    }

}
