/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <list>

#include <engine/declarations.hpp>
#include <engine/Object_Pool.hpp>
#include <engine/Registrar.hpp>
#include <engine/Sparse_Array.hpp>

namespace udit::engine
{

    class Subsystem : public Registrar< Subsystem >
    {
    public:

        using Unique_Ptr = std::unique_ptr< Subsystem >;

    protected:

        template< class COMPONENT >
        class Component_Store
        {
            using Component         = COMPONENT;
            //using Object_Pool       = engine:: Object_Pool< Component >;
            //using Component_Pointer = typename Object_Pool::Pointer;

            using Object_Pool       = std::list< Component >;
            using Component_Pointer = Component *;

        private:

            Object_Pool pool;

            Sparse_Array< Component_Pointer > components_by_entity;

        public:

            Component * get (Id entity_id)
            {
                auto & entry = components_by_entity[entity_id];

                //return entry.has_value () ? entry.value ().get () : nullptr;
                return entry.has_value () ? entry.value () : nullptr;
            }

            Object_Pool::iterator begin ()
            {
                return pool.begin ();
            }

            Object_Pool::iterator end ()
            {
                return pool.end ();
            }

        public:

            /*template< typename ...ARGUMENTS >
            Component * allocate (Id entity_id, const ARGUMENTS & ...arguments)
            {
                auto component = pool.allocate (arguments...);

                component->entity_id = entity_id;

                components_by_entity[entity_id] = component;

                return component.get ();
            }*/

            template< typename ...ARGUMENTS >
            Component * allocate (Id entity_id, const ARGUMENTS & ...arguments)
            {
                pool.emplace_back (arguments...);

                auto component = &pool.back ();

                component->entity_id = entity_id;

                components_by_entity[entity_id] = component;

                return component;
            }
        };

    public:

        template< class SUBSYSTEM >
        static Id setup ();

        template< class SUBSYSTEM >
        static Id id_of ();

        template< class SUBSYSTEM >
        static Unique_Ptr create (Scene & scene);

        static Unique_Ptr create (Id subsystem_id, Scene & scene)
        {
            return registry ().metadata_by_id (subsystem_id)->factory (scene);
        }

    protected:

        Scene & scene;

    public:

        Subsystem(Scene & host_scene) : scene(host_scene)
        {
        }

        virtual ~Subsystem() = default;

    public:

        template< class COMPONENT >
        Component * get_component (Id entity_id);

        template< class COMPONENT, typename ...ARGUMENTS >
        Component * create_component (Entity & entity, ARGUMENTS && ...arguments);

    };

}
