/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <chrono>
#include <thread>
#include <vector>

#include <engine/Kernel.hpp>
#include <engine/Timer.hpp>

namespace udit::engine
{

    void Kernel::run ()
    {
        running    = true;
        stop_token = false;

        for (auto & stage : pipeline) stage->prepare ();

        float frame_duration = 1.f / 60.f;

        while (not stop_token)
        {
            Timer timer;

            for (auto & stage : pipeline)
            {
                stage->compute (frame_duration);

                if (stop_token) break;
            }

            frame_duration = timer.get_elapsed< Seconds > ();
        }

        for (auto & stage : pipeline) stage->cleanup ();

        running = false;
    }

}
