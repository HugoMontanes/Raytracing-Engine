/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <engine/Input_Event.hpp>
#include <engine/Key_Code.hpp>

namespace udit::engine
{

    struct Key_Event : public Input_Event
    {
        enum State
        {
            PRESSED, RELEASED
        };

    public:

        Key_Code code;
        State    state;

    public:

        Key_Event(Queue_Pool_Base * pool, Key_Code given_code, State given_state)
        :
            Input_Event(pool       ),
            code       (given_code ),
            state      (given_state)
        {
        }
    };

}
