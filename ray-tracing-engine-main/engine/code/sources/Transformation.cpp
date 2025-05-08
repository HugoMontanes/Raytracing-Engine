/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <typeinfo>
#include <engine/Transformation.hpp>

namespace udit::engine
{

    template<>
    Subsystem::Unique_Ptr Subsystem::create< Transformation > (Scene & scene)
    {
        return std::make_unique< Transformation > (scene);
    }

    template<>
    Stage::Unique_Ptr Stage::create< Transformation > (Scene & scene)
    {
        return std::make_unique< Transformation::Stage > (scene);
    }

    template<>
    template<>
    Id Registrar< Subsystem >::record< Transformation > ()
    {
        return registry ().add ("Transformation", Subsystem::create< Transformation >);
    }

    template<>
    template<>
    Id Registrar< Stage >::record< Transformation > ()
    {
        return registry ().add ("Transformation::Stage", Stage::create< Transformation >);
    }

    template<>
    Id Subsystem::setup< Transformation > ()
    {
        static Id id = INVALID_ID;

        if (not_valid (id))
        {
            id = Subsystem::record< Transformation > ();
        }

        return id;
    }

    template<>
    Id Subsystem::id_of< Transformation > ()
    {
        return Subsystem::setup< Transformation > ();
    }

    template<>
    Id Stage::setup< Transformation > ()
    {
        static Id id = INVALID_ID;

        if (not_valid (id))
        {
            id = Stage::record< Transformation > ();
        }

        return id;
    }

    template<>
    Id Stage::id_of< Transformation > ()
    {
        return Stage::setup< Transformation > ();
    }

    template<>
    Id Subsystem::id_of< Transform > ()
    {
        return Subsystem::setup< Transformation > ();
    }

    template< >
    Component * Subsystem::get_component< Transform > (Id entity_id)
    {
        return static_cast< Transformation * >(this)->get_transform_component (entity_id);
    }

    template< >
    Component * Subsystem::create_component< Transform > (Entity & entity)
    {
        return static_cast< Transformation * >(this)->create_transform_component (entity);
    }

    template< >
    Component * Subsystem::create_component< Transform >
    (
        Entity & entity,
        const Vector3 & position,
        const Vector3 & rotation,
        const Vector3 & scales
    )
    {
        return static_cast< Transformation * >(this)->create_transform_component (entity, position, rotation, scales);
    }

}
