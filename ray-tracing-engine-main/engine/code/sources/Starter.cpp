/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#define SDL_MAIN_HANDLED

#include <mutex>
#include <engine/Starter.hpp>
#include <SDL3/SDL_main.h>

namespace udit::engine
{

    Starter & starter = Starter::instance ();

    void Starter::run (const std::function< void() > & runnable)
    {
        static std::mutex mutex;

        std::lock_guard lock(mutex);

        if (initialize ())
        {
            Finalizer finalizer;

            runnable ();
        }
    }

    bool Starter::initialize ()
    {
        static int run_before = 0;

        if (not run_before++)
        {
            SDL_SetMainReady ();
        }

        return SDL_Init (0);
    }

    Starter::Finalizer::~Finalizer()
    {
        SDL_Quit ();
    }

}
