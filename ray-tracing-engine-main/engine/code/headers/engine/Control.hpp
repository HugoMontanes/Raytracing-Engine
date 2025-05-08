/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <functional>
#include <memory>

#include <engine/Controller.hpp>
#include <engine/Entity.hpp>
#include <engine/Stage.hpp>
#include <engine/Subsystem.hpp>
#include <engine/Transform.hpp>

namespace udit::engine
{

    class Control : public Subsystem
    {
    public:

        class Stage : public engine::Stage
        {
            Control * subsystem;

        public:

            Stage(Scene & scene) : engine::Stage(scene)
            {
                subsystem = nullptr;
            }

            void prepare ()      override;
            void compute (float) override;
        };

        friend class Stage;

    public:

        struct Component : public engine::Component
        {
            std::shared_ptr< Controller > controller;
        };

    private:

        Component_Store< Component > controller_components;

    public:

        Control(Scene & scene) : Subsystem(scene)
        {
        }

    public:

        Component * create_controller_component (Entity & entity, const std::shared_ptr< Controller > & control_function);

    };

}
