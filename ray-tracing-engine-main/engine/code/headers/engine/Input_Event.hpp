/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <queue>
#include <typeinfo>

#include <engine/Timer.hpp>

namespace udit::engine
{

    class Input_Event
    {
    public:

        class Queue
        {
            using Input_Event_Queue = std::queue< Input_Event * >;

        private:

            Input_Event_Queue queue;

        public:

            size_t length () const
            {
                return queue.size ();
            }

            void clear ()
            {
                Input_Event_Queue empty;
                queue.swap (empty);
            }

            void push (Input_Event * event)
            {
                queue.push (event);
            }

            Input_Event * poll ()
            {
                if (queue.empty ()) return nullptr;

                auto event = queue.front ();

                queue.pop ();

                return event;
            }
        };

    protected:

        struct Queue_Pool_Base
        {
            virtual ~Queue_Pool_Base() = default;

        public:

            virtual void pop () = 0;
        };

    public:

        template< class INPUT_EVENT_TYPE >
        class Queue_Pool : public Queue_Pool_Base
        {
            using Input_Event_Type = INPUT_EVENT_TYPE;
            using Input_Event_Pool = std::queue< Input_Event_Type >;

        private:

            Input_Event_Pool  pool;

        public:

            void clear ()
            {
                Input_Event_Pool empty;

                pool.swap (empty);
            }

            template< typename ...ARGUMENTS >
            Input_Event_Type * push (const ARGUMENTS & ...arguments)
            {
                pool.emplace (this, arguments...);

                return &pool.back ();
            }

            void pop () override
            {
                if (not pool.empty ()) pool.pop ();
            }
        };

    private:

        static uint64_t current_timestamp ()
        {
            static Timer timer;
            return timer.get_elapsed< Milliseconds > ();
        }

        Queue_Pool_Base * const pool;

    public:

        const uint64_t timestamp;

    public:

        Input_Event(Queue_Pool_Base * host_pool)
        :
            pool     (host_pool),
            timestamp(current_timestamp())
        {
        }

        virtual ~Input_Event() = default;

    public:

        template< class INPUT_EVENT_TYPE >
        bool is () const
        {
            return typeid(this) == typeid(INPUT_EVENT_TYPE);
        }

        template< class INPUT_EVENT_TYPE >
        const INPUT_EVENT_TYPE * as () const
        {
            return dynamic_cast< const INPUT_EVENT_TYPE * >(this);
        }

    };

}
