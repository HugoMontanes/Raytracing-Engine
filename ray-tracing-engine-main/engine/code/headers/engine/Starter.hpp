/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */
#pragma once

#include <functional>

namespace udit::engine
{

    class Starter
    {
    public:

        static Starter & instance ()
        {
            static Starter engine;
            return engine;
        }

    public:

        void run (const std::function< void() > & runnable);

    private:

        bool initialize ();

        struct Finalizer
        {
            ~Finalizer();
        };

    };

    extern Starter & starter;

}
