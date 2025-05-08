/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <engine/Stage.hpp>

namespace udit::engine
{

    class Display_Stage : public Stage
    {
    public:

        Display_Stage(Scene & scene) : Stage(scene)
        {
        }

        void compute (float time) override;

    };

}
