/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <map>
#include <memory>
#include <vector>

#include <engine/Entity.hpp>
#include <engine/Id_Provider.hpp>
#include <engine/Input_Event.hpp>
#include <engine/Kernel.hpp>
#include <engine/Sparse_Array.hpp>
#include <engine/Stage.hpp>
#include <engine/Subsystem.hpp>

namespace udit::engine
{

    class Scene
    {
        using Subsystem_Array = std::vector < Subsystem::Unique_Ptr >;
        using Entity_Array    = Sparse_Array< Entity   >;
        using Entity_Map      = std::map< Id, Entity * >;

    public:

        Subsystem_Array    subsystems;
        Kernel             kernel;

        Entity_Array       entities;
        Entity_Map         entities_by_name;
        Id_Provider        entities_id_provider;

        Window           & window;
        Input_Event::Queue input_event_queue;

    public:

        Scene(Window & reference_window)
        :
            subsystems(16),
            window(reference_window)
        {
            add< Input_Stage   > ();
            add< Control       > ();
            add< Path_Tracing  > ();
            add< Display_Stage > ();
        }

    public:

        Window & get_window ()
        {
            return window;
        }

        Input_Event::Queue & get_input_event_queue ()
        {
            return input_event_queue;
        }

        template< class COMPONENT >
        COMPONENT * get_component (const Entity & entity)
        {
            auto & subsystem = subsystems[Subsystem::id_of< COMPONENT > ()];

            return subsystem ? dynamic_cast< COMPONENT * >(subsystem->template get_component< COMPONENT > (entity.id)) : nullptr;
        }

        template< class COMPONENT >
        COMPONENT * get_component (Id entity_id)
        {
            auto & subsystem = subsystems[Subsystem::id_of< COMPONENT > ()];

            return subsystem ? dynamic_cast< COMPONENT * >(subsystem->template get_component< COMPONENT > (entity_id)) : nullptr;
        }

        template< class SUBSYSTEM >
        SUBSYSTEM * get_subsystem ()
        {
            return dynamic_cast< SUBSYSTEM * >
            (
                subsystems[Subsystem::id_of< SUBSYSTEM > ()].get ()
            );
        }

    public:

        template< class STAGE >
        Stage & add ()
        {
            return kernel.add< STAGE > (*this);
        }

        Entity & create_entity ()
        {
            Id entity_id = entities_id_provider.allocate_id ();

            return (entities[entity_id] = Entity{entity_id}).value ();
        }

        Entity & create_entity (Id name)
        {
            auto & entity = create_entity ();

            entities_by_name[name] = &entity;

            return entity;
        }

        template< class COMPONENT, typename ...ARGUMENTS >
        COMPONENT * create_component (Entity & entity, const ARGUMENTS & ...arguments)
        {
            auto   subsystem_id = Subsystem::id_of< COMPONENT > ();
            auto & subsystem    = subsystems[subsystem_id];

            if (not subsystem)
            {
                initialize (subsystem, subsystem_id);
            }

            return static_cast< COMPONENT * >(subsystem->template create_component< COMPONENT > (entity, arguments...));
        }

    public:

        void run ()
        {
            kernel.run ();
        }

        void stop ()
        {
            kernel.stop ();
        }

    private:

        void initialize (Subsystem::Unique_Ptr & subsystem, Id subsystem_id)
        {
            subsystem = Subsystem::create (subsystem_id, *this);
        }

    };

}
