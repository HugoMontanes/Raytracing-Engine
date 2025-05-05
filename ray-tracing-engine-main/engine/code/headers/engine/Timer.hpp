/*
 * Copyright © 2017+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <chrono>

namespace udit::engine
{

    class Timer
    {
        using Clock     = std::chrono::high_resolution_clock;
        using Timestamp = std::chrono::time_point< Clock >;

    private:

        Timestamp start;

    public:

        Timer()
        {
            reset ();
        }

        void reset ()
        {
            start = Clock::now ();
        }

        template< typename UNIT >
        auto get_elapsed ()
        {
            return std::chrono::duration_cast< UNIT >(Clock::now () - start).count ();
        }
    };

    using Nanoseconds  = std::chrono::nanoseconds;
    using Microseconds = std::chrono::microseconds;
    using Milliseconds = std::chrono::milliseconds;
    using Seconds      = std::chrono::duration< float >;

    using namespace      std::chrono_literals;

}
