/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <engine/Controller.hpp>
#include <engine/declarations.hpp>
#include <engine/math.hpp>

namespace udit
{

    class Camera_Controller : public engine::Controller
    {
        engine::Transform * transform;
        engine::Vector3     direction;
        engine::Vector3     speed;

    public:

        Camera_Controller(engine::Scene & scene, engine::Id entity_id);

        void update (float frame_time) override;

    private:

        void update_direction ();

    };

}
