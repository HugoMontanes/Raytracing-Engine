/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <engine/Id.hpp>

namespace udit::engine
{

    struct Component
    {
        Id entity_id = INVALID_ID;

        virtual ~Component() = default;
    };

}
