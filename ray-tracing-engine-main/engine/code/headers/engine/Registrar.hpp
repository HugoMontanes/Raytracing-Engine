/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <engine/Registry.hpp>

namespace udit::engine
{

    template< class CLASS >
    class Registrar
    {
    protected:

        using  Registry =   engine::Registry< CLASS, 16 >;
        using  Metadata = Registry::Metadata;

        static Registry & registry ()
        {
            static Registry registry;
            return registry;
        }

    protected:

        template< class >
        static Id record ();

    public:

        virtual ~Registrar() = default;

    };

}
