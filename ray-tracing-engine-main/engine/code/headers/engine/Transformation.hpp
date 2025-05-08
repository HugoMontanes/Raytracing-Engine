/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <memory>

#include <engine/Entity.hpp>
#include <engine/Stage.hpp>
#include <engine/Subsystem.hpp>
#include <engine/Transform.hpp>

namespace udit::engine
{

    class Transformation : public Subsystem
    {
    public:

        class Stage : public engine::Stage
        {
        public:

            Stage(Scene & scene) : engine::Stage(scene)
            {
            }

            void prepare ()      override { }
            void compute (float) override { }
            void cleanup ()      override { }
        };

    private:

        Component_Store< Transform > transform_components;

    public:

        Transformation(Scene & scene) : Subsystem(scene)
        {
        }

    public:

        Component * get_transform_component (Id entity_id)
        {
            return transform_components.get (entity_id);
        }

    public:

        Component * create_transform_component (Entity & entity)
        {
            return transform_components.allocate (entity.id);
        }

        Component * create_transform_component (Entity & entity, const Vector3 & position, const Vector3 & rotation, const Vector3 & scales)
        {
            return transform_components.allocate (entity.id, position, rotation, scales);
        }

    };

}
