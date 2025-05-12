// engine/code/sources/Input_Stage.cpp
#include <engine/Input_Stage.hpp>
#include <engine/Scene.hpp>
#include <SDL3/SDL.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace udit::engine
{
    namespace internal
    {
        // Debug helper functions
        std::string get_timestamp()
        {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;

            std::stringstream ss;

#ifdef _WIN32
            // Use localtime_s for Windows
            struct tm tm_info;
            localtime_s(&tm_info, &time_t);
            ss << std::put_time(&tm_info, "%H:%M:%S");
#else
            // Use localtime for other platforms
            ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
#endif

            ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
            return ss.str();
        }

        std::string get_thread_id()
        {
            std::stringstream ss;
            ss << std::this_thread::get_id();
            return ss.str();
        }

        // Debug wrapper for logging
        void debug_log(const std::string& function, const std::string& message)
        {
            std::cout << "[" << get_timestamp() << "] [Thread "
                << get_thread_id() << "] "
                << function << ": " << message << std::endl;
        }

        // Your existing key_code_from_sdl_key_code function
        static Key_Code key_code_from_sdl_key_code(SDL_Keycode sdl_key_code)
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

    // Remove the constructor definition since it's already inline in the header
    // Constructor is already defined in the header as:
    // Input_Stage(Scene & scene) : Stage(scene) {}

    void Input_Stage::prepare()
    {
        internal::debug_log("Input_Stage::prepare", "Starting preparation");

        // Clear any existing state to ensure clean start
        scene.get_input_event_queue().clear();
        key_events.clear();

        internal::debug_log("Input_Stage::prepare", "Cleared scene and key event queues");

        // Clear pending events
        {
            std::lock_guard<std::mutex> lock(events_mutex);
            std::queue<Event_Data> empty;
            pending_events.swap(empty);
            internal::debug_log("Input_Stage::prepare", "Cleared pending events queue");
        }

        internal::debug_log("Input_Stage::prepare", "Preparation completed");
    }

    void Input_Stage::compute(float delta_time)
    {
        static int frame_count = 0;
        static auto last_log_time = std::chrono::steady_clock::now();

        frame_count++;

        // Log every second
        auto now = std::chrono::steady_clock::now();
        auto seconds_elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_log_time).count();

        if (seconds_elapsed >= 1)
        {
            std::stringstream msg;
            msg << "Frame " << frame_count << " (delta: " << delta_time << "s, FPS: "
                << (frame_count / (seconds_elapsed > 0 ? seconds_elapsed : 1)) << ")";
            internal::debug_log("Input_Stage::compute", msg.str());

            frame_count = 0;
            last_log_time = now;
        }

        SDL_Event event;
        int event_count = 0;

        // Poll all SDL events on the main thread
        while (SDL_PollEvent(&event))
        {
            event_count++;

            switch (event.type)
            {
            case SDL_EVENT_KEY_DOWN:
            {
                auto key_code = internal::key_code_from_sdl_key_code(event.key.key);

                std::stringstream msg;
                msg << "KEY_DOWN - code: " << key_code
                    << ", SDL key: " << event.key.key;
                internal::debug_log("Input_Stage::compute", msg.str());

                Event_Data data{ key_code, Key_Event::PRESSED };

                {
                    std::lock_guard<std::mutex> lock(events_mutex);
                    pending_events.push(data);
                    internal::debug_log("Input_Stage::compute",
                        "Added KEY_DOWN to pending_events (size: " +
                        std::to_string(pending_events.size()) + ")");
                }
                break;
            }

            case SDL_EVENT_KEY_UP:
            {
                auto key_code = internal::key_code_from_sdl_key_code(event.key.key);

                std::stringstream msg;
                msg << "KEY_UP - code: " << key_code
                    << ", SDL key: " << event.key.key;
                internal::debug_log("Input_Stage::compute", msg.str());

                Event_Data data{ key_code, Key_Event::RELEASED };

                {
                    std::lock_guard<std::mutex> lock(events_mutex);
                    pending_events.push(data);
                    internal::debug_log("Input_Stage::compute",
                        "Added KEY_UP to pending_events (size: " +
                        std::to_string(pending_events.size()) + ")");
                }
                break;
            }

            case SDL_EVENT_QUIT:
            {
                internal::debug_log("Input_Stage::compute", "QUIT event received");
                scene.stop();
                break;
            }

            default:
            {
                // Log other event types for debugging
                if (event_count == 1) // Only log first few to avoid spam
                {
                    std::stringstream msg;
                    msg << "Other SDL event type: " << event.type;
                    internal::debug_log("Input_Stage::compute", msg.str());
                }
                break;
            }
            }
        }

        if (event_count > 0)
        {
            std::stringstream msg;
            msg << "Polled " << event_count << " SDL events this frame";
            internal::debug_log("Input_Stage::compute", msg.str());
        }

        // Process the accumulated events
        process_pending_events();
    }

    void Input_Stage::process_pending_events()
    {
        std::queue<Event_Data> events_to_process;
        size_t original_size = 0;

        // Quickly swap the queues to minimize lock time
        {
            std::lock_guard<std::mutex> lock(events_mutex);
            original_size = pending_events.size();

            if (original_size > 0)
            {
                events_to_process.swap(pending_events);

                std::stringstream msg;
                msg << "Swapped " << original_size << " events from pending_events";
                internal::debug_log("Input_Stage::process_pending_events", msg.str());
            }
        }

        if (original_size == 0)
        {
            return; // Nothing to process
        }

        // Process all events
        int processed_count = 0;
        while (!events_to_process.empty())
        {
            const auto& event = events_to_process.front();

            // Push to the scene's input event queue
            scene.get_input_event_queue().push(
                key_events.push(event.code, event.state)
            );

            std::stringstream msg;
            msg << "Pushed event to scene queue - code: " << event.code
                << ", state: " << (event.state == Key_Event::PRESSED ? "PRESSED" : "RELEASED");
            internal::debug_log("Input_Stage::process_pending_events", msg.str());

            processed_count++;
            events_to_process.pop();
        }

        std::stringstream final_msg;
        final_msg << "Processed " << processed_count << " events total";
        internal::debug_log("Input_Stage::process_pending_events", final_msg.str());
    }

    void Input_Stage::cleanup()
    {
        internal::debug_log("Input_Stage::cleanup", "Starting cleanup");

        scene.get_input_event_queue().clear();
        internal::debug_log("Input_Stage::cleanup", "Cleared scene input event queue");

        key_events.clear();
        internal::debug_log("Input_Stage::cleanup", "Cleared key events");

        // Clear pending events
        {
            std::lock_guard<std::mutex> lock(events_mutex);
            std::queue<Event_Data> empty;
            pending_events.swap(empty);
            internal::debug_log("Input_Stage::cleanup", "Cleared pending events");
        }

        internal::debug_log("Input_Stage::cleanup", "Cleanup completed");
    }

    // Keep the existing registration code unchanged
    template<>
    Stage::Unique_Ptr Stage::create< Input_Stage >(Scene& scene)
    {
        return std::make_unique< Input_Stage >(scene);
    }

    template<>
    template<>
    Id Registrar< Stage >::record< Input_Stage >()
    {
        return registry().add("Input_Stage", Stage::create< Input_Stage >);
    }

    template<>
    Id Stage::setup< Input_Stage >()
    {
        static Id id = INVALID_ID;

        if (not_valid(id))
        {
            id = Stage::record< Input_Stage >();
        }

        return id;
    }

    template<>
    Id Stage::id_of< Input_Stage >()
    {
        return Stage::setup< Input_Stage >();
    }
}