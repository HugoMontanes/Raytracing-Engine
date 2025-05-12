/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <engine/Input_Stage.hpp>
#include <engine/Scene.hpp>
#include <engine/Thread_Pool_Manager.hpp>

#include <iostream>
#include <SDL3/SDL.h>

namespace udit::engine
{

    namespace internal
    {

        static Key_Code key_code_from_sdl_key_code (SDL_Keycode sdl_key_code)
        {
            switch (sdl_key_code)
            {
                case SDLK_A:     return KEY_A;
                case SDLK_B:     return KEY_B;
                case SDLK_C:     return KEY_C;
                case SDLK_D:     return KEY_D;
                case SDLK_E:     return KEY_E;
                case SDLK_F:     return KEY_F;
                case SDLK_G:     return KEY_G;
                case SDLK_H:     return KEY_H;
                case SDLK_I:     return KEY_I;
                case SDLK_J:     return KEY_J;
                case SDLK_K:     return KEY_K;
                case SDLK_L:     return KEY_L;
                case SDLK_M:     return KEY_M;
                case SDLK_N:     return KEY_N;
                case SDLK_O:     return KEY_O;
                case SDLK_P:     return KEY_P;
                case SDLK_Q:     return KEY_Q;
                case SDLK_R:     return KEY_R;
                case SDLK_S:     return KEY_S;
                case SDLK_T:     return KEY_T;
                case SDLK_U:     return KEY_U;
                case SDLK_V:     return KEY_V;
                case SDLK_W:     return KEY_W;
                case SDLK_X:     return KEY_X;
                case SDLK_Y:     return KEY_Y;
                case SDLK_Z:     return KEY_Z;
                case SDLK_0:     return KEY_0;
                case SDLK_1:     return KEY_1;
                case SDLK_2:     return KEY_2;
                case SDLK_3:     return KEY_3;
                case SDLK_4:     return KEY_4;
                case SDLK_5:     return KEY_5;
                case SDLK_6:     return KEY_6;
                case SDLK_7:     return KEY_7;
                case SDLK_8:     return KEY_8;
                case SDLK_9:     return KEY_9;
                case SDLK_LEFT:  return KEY_LEFT;
                case SDLK_RIGHT: return KEY_RIGHT;
                case SDLK_UP:    return KEY_UP;
                case SDLK_DOWN:  return KEY_DOWN;
            }

            return UNDEFINED;
        }

    }

    template<>
    Stage::Unique_Ptr Stage::create< Input_Stage > (Scene & scene)
    {
        return std::make_unique< Input_Stage > (scene);
    }

    template<>
    template<>
    Id Registrar< Stage >::record< Input_Stage > ()
    {
        return registry ().add ("Input_Stage", Stage::create< Input_Stage >);
    }

    template<>
    Id Stage::setup< Input_Stage > ()
    {
        static Id id = INVALID_ID;

        if (not_valid (id))
        {
            id = Stage::record< Input_Stage > ();
        }

        return id;
    }

    template<>
    Id Stage::id_of< Input_Stage > ()
    {
        return Stage::setup< Input_Stage > ();
    }

    Input_Stage::~Input_Stage()
    {
        stop_input_processing();
    }


    void Input_Stage::compute (float)
    {
        // Check if the input processing task is running
        std::lock_guard<std::mutex> lock(input_task_mutex);

        if (!input_processing_active && input_task_future.valid())
        {
            // Check if the task has completed unexpectedly
            auto status = input_task_future.wait_for(std::chrono::milliseconds(0));

            if (status == std::future_status::ready)
            {
                try {
                    // Get the result to catch any exceptions
                    input_task_future.get();
                }
                catch (const std::exception& e) {
                    std::cerr << "Input processing task failed: " << e.what() << std::endl;
                }

                // Restart the input processing
                start_input_processing();
            }
        }
        else if (!input_processing_active)
        {
            // Start the input processing if it's not running
            start_input_processing();
        }
    }

    void Input_Stage::cleanup ()
    {
        stop_input_processing();
        scene.get_input_event_queue().clear();
        key_events.clear();
    }

    void Input_Stage::prepare()
    {
        start_input_processing();
    }

    void Input_Stage::process_input_continuously()
    {
        SDL_Event event;

        while (input_processing_active)
        {
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_EVENT_KEY_DOWN:
                    {
                        scene.get_input_event_queue().push(
                            key_events.push(
                                internal::key_code_from_sdl_key_code(event.key.key),
                                Key_Event::PRESSED
                            )
                        );
                        break;

                    }

                    case SDL_EVENT_KEY_UP:
                    {

                        scene.get_input_event_queue().push(
                            key_events.push(
                                internal::key_code_from_sdl_key_code(event.key.key),
                                Key_Event::RELEASED
                            )
                        );
                        break;
                    }
                }
            }
            //Small sleep to prevent CPU hogging
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    void Input_Stage::start_input_processing()
    {
        std::lock_guard<std::mutex> lock(input_task_mutex);

        if (!input_processing_active)
        {
            input_processing_active = true;

            //Get the input thread pool
            auto& input_pool = Thread_Pool_Manager::get_instance().get_pool(Thread_Pool_Type::INPUT);

            //Submit the continuous input processing task
            input_task_future = input_pool.submit(
                Task_Priority::HIGH,
                [this]() {process_input_continuously(); }
            );
        }
    }

    void Input_Stage::stop_input_processing()
    {
        std::lock_guard<std::mutex> lock(input_task_mutex);

        if (input_processing_active)
        {
            input_processing_active = false;

            //Wait for the task to finish
            if (input_task_future.valid())
            {
                try
                {
                    input_task_future.wait();
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Exception in input processing task: " << e.what() << std::endl;
                }
            }
        }
    }

}
