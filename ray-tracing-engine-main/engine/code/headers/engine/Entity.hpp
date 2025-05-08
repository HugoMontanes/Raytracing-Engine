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

    struct Entity
    {
        Id id;

    public:

        Entity(const Id given_id) : id(given_id)
        {
        }
    };

}
