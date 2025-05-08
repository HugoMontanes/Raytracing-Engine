/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <engine/Control.hpp>
#include <engine/Scene.hpp>

namespace udit::engine
{

    template<>
    Subsystem::Unique_Ptr Subsystem::create< Control > (Scene & scene)
    {
        return std::make_unique< Control > (scene);
    }

    template<>
    Stage::Unique_Ptr Stage::create< Control > (Scene & scene)
    {
        return std::make_unique< Control::Stage > (scene);
    }

    template<>
    template<>
    Id Registrar< Subsystem >::record< Control > ()
    {
        return registry ().add ("Control", Subsystem::create< Control >);
    }

    template<>
    template<>
    Id Registrar< Stage >::record< Control > ()
    {
        return registry ().add ("Control::Stage", Stage::create< Control >);
    }

    template<>
    Id Subsystem::setup< Control > ()
    {
        static Id id = INVALID_ID;

        if (not_valid (id))
        {
            id = Subsystem::record< Control > ();
        }

        return id;
    }

    template<>
    Id Subsystem::id_of< Control > ()
    {
        return Subsystem::setup< Control > ();
    }

    template<>
    Id Stage::setup< Control > ()
    {
        static Id id = INVALID_ID;

        if (not_valid (id))
        {
            id = Stage::record< Control > ();
        }

        return id;
    }

    template<>
    Id Stage::id_of< Control > ()
    {
        return Stage::setup< Control > ();
    }

    template<>
    Id Subsystem::id_of< Control::Component > ()
    {
        return Subsystem::setup< Control > ();
    }

    template< >
    Component * Subsystem::create_component< Control::Component > (Entity & entity, const std::shared_ptr< Controller > & control_function)
    {
        return static_cast< Control * >(this)->create_controller_component (entity, control_function);
    }

    Control::Component * Control::create_controller_component (Entity & entity, const std::shared_ptr< Controller > & control_function)
    {
        auto controller = controller_components.allocate (entity.id);

        controller->controller = control_function;

        return controller;
    }

    void Control::Stage::prepare ()
    {
        subsystem = scene.get_subsystem< Control > ();
    }

    void Control::Stage::compute (float frame_time)
    {
        if (subsystem)
        {
            for (auto & component : subsystem->controller_components)
            {
                component.controller->update (frame_time);
            }
        }
    }

}
