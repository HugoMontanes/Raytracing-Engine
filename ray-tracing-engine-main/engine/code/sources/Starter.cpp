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
        unsigned rendering_threads = std::max<size_t>(1u, hardware_threads - 2);
        unsigned input_threads = 1;         // Single thread for input processing is usually sufficient
        unsigned general_threads = 1;       // For miscellaneous tasks

        Thread_Pool_Manager::get_instance().initialize(
            general_threads,  // General purpose
            rendering_threads, // Rendering
            1,                // Loading (2 threads for asset loading)
            input_threads                 // Input (1 dedicated thread)
        );

        return SDL_Init (0);
    }

    Starter::Finalizer::~Finalizer()
    {
        SDL_Quit ();
        Thread_Pool_Manager::get_instance().shutdown();
    }

}
