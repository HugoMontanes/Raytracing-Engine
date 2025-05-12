/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <engine/Key_Event.hpp>
#include <engine/Stage.hpp>
#include <engine/Timer.hpp>
#include <iostream>
#include <atomic>
#include <future>

namespace udit::engine
{

    class Input_Stage : public Stage
    {
        using Key_Event_Pool = Input_Event::Queue_Pool< Key_Event >;

    private:

        Key_Event_Pool key_events;

        // Event processing queue
        struct Event_Data {
            Key_Code code;
            Key_Event::State state;
        };

        std::queue<Event_Data> pending_events;
        std::mutex events_mutex;

    public:

        Input_Stage(Scene& scene)
            :Stage(scene){}

        void prepare() override;

        void compute (float) override;

        void cleanup () override;

    private:

        void process_pending_events();
    };

}
