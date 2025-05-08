/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <vector>
#include <engine/Stage.hpp>

namespace udit::engine
{

    class Kernel
    {
        using Stage_Array = std::vector< Stage::Unique_Ptr >;
        using Pipeline    = std::vector< Stage * >;

    private:

        Stage_Array stages;
        Pipeline    pipeline;

        bool running;
        bool stop_token;

    public:

        Kernel() : stages(16)
        {
            running    = false;
            stop_token = false;
        }

       ~Kernel()
        {
           stop ();
        }

    public:

        template< class STAGE >
        Stage & add (Scene & scene);

        template< class STAGE >
        Stage * get ()
        {
            return stages[Stage::id_of< STAGE > ()].get ();
        }

    public:

        void run ();

        void stop ()
        {
            stop_token = true;
        }
    };


    template< class STAGE >
    Stage & Kernel::add (Scene & scene)
    {
        auto & stage = stages[Stage::id_of< STAGE > ()];

        if (not stage)
        {
            stage = Stage::create< STAGE > (scene);

            pipeline.push_back (stage.get ());
        }

        return *stage;
    }


}
