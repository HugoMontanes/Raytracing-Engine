/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <engine/declarations.hpp>
#include <engine/Registrar.hpp>

namespace udit::engine
{

    class Stage : public Registrar< Stage >
    {
    public:

        using Unique_Ptr = std::unique_ptr< Stage >;

    public:

        template< class STAGE >
        static Id setup ();

        template< class STAGE >
        static Id id_of ();

        template< class STAGE >
        static Unique_Ptr create (Scene & scene);

    protected:

        Scene & scene;

    public:

        Stage(Scene & host_scene) : scene(host_scene)
        {
        }

        virtual ~Stage() = default;

    public:

        virtual void prepare () { }
        virtual void cleanup () { }
        virtual void compute (float frame_time) = 0;

    };

}
