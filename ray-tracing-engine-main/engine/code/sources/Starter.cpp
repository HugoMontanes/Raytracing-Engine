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
#include <algorithm>
#include <cstddef>
#include "engine/Thread_Pool_Manager.hpp"

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

        size_t hardware_threads = std::thread::hardware_concurrency();
        size_t general_threads = std::max<size_t>(2u, hardware_threads / 4);
        size_t rendering_threads = std::max<size_t>(1u, hardware_threads - general_threads - 1);

        Thread_Pool_Manager::get_instance().initialize(
            general_threads,  // General purpose
            rendering_threads, // Rendering
            2,                // Loading (2 threads for asset loading)
            1                 // Input (1 dedicated thread)
        );

        return SDL_Init (0);
    }

    Starter::Finalizer::~Finalizer()
    {
        SDL_Quit ();
        Thread_Pool_Manager::get_instance().shutdown();
    }

}
