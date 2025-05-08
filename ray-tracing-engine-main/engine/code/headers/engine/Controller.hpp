/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <engine/declarations.hpp>
#include <engine/Id.hpp>

namespace udit::engine
{

    class Controller
    {
    protected:

        Scene & scene;
        Id  entity_id;

    public:

        Controller(Scene & given_scene, Id given_entity_id)
        :
            scene    (given_scene    ),
            entity_id(given_entity_id)
        {
        }

        virtual ~Controller() = default;

    public:

        virtual void update (float frame_time) = 0;
    };

}
