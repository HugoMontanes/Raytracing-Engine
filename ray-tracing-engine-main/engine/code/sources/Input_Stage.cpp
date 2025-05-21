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
    }

    // Convert SDL scancode to our Key_Code enum
    Key_Code Input_Stage::scancode_to_key_code(SDL_Scancode scancode)
    {
        switch (scancode)
        {
        case SDL_SCANCODE_A: return KEY_A;
        case SDL_SCANCODE_B: return KEY_B;
        case SDL_SCANCODE_C: return KEY_C;
        case SDL_SCANCODE_D: return KEY_D;
        case SDL_SCANCODE_E: return KEY_E;
        case SDL_SCANCODE_F: return KEY_F;
        case SDL_SCANCODE_G: return KEY_G;
        case SDL_SCANCODE_H: return KEY_H;
        case SDL_SCANCODE_I: return KEY_I;
        case SDL_SCANCODE_J: return KEY_J;
        case SDL_SCANCODE_K: return KEY_K;
        case SDL_SCANCODE_L: return KEY_L;
        case SDL_SCANCODE_M: return KEY_M;
        case SDL_SCANCODE_N: return KEY_N;
        case SDL_SCANCODE_O: return KEY_O;
        case SDL_SCANCODE_P: return KEY_P;
        case SDL_SCANCODE_Q: return KEY_Q;
        case SDL_SCANCODE_R: return KEY_R;
        case SDL_SCANCODE_S: return KEY_S;
        case SDL_SCANCODE_T: return KEY_T;
        case SDL_SCANCODE_U: return KEY_U;
        case SDL_SCANCODE_V: return KEY_V;
        case SDL_SCANCODE_W: return KEY_W;
        case SDL_SCANCODE_X: return KEY_X;
        case SDL_SCANCODE_Y: return KEY_Y;
        case SDL_SCANCODE_Z: return KEY_Z;
        case SDL_SCANCODE_0: return KEY_0;
        case SDL_SCANCODE_1: return KEY_1;
        case SDL_SCANCODE_2: return KEY_2;
        case SDL_SCANCODE_3: return KEY_3;
        case SDL_SCANCODE_4: return KEY_4;
        case SDL_SCANCODE_5: return KEY_5;
        case SDL_SCANCODE_6: return KEY_6;
        case SDL_SCANCODE_7: return KEY_7;
        case SDL_SCANCODE_8: return KEY_8;
        case SDL_SCANCODE_9: return KEY_9;
        case SDL_SCANCODE_LEFT: return KEY_LEFT;
        case SDL_SCANCODE_RIGHT: return KEY_RIGHT;
        case SDL_SCANCODE_UP: return KEY_UP;
        case SDL_SCANCODE_DOWN: return KEY_DOWN;
        default: return UNDEFINED;
        }
    }

    void Input_Stage::prepare()
    {
        internal::debug_log("Input_Stage::prepare", "Starting preparation");

        // Clear any existing state
        scene.get_input_event_queue().clear();
        key_events.clear();

        // Initialize keyboard state tracking
        current_key_state = SDL_GetKeyboardState(&num_keys);
        previous_key_state.resize(num_keys, 0);

        internal::debug_log("Input_Stage::prepare", "Preparation completed with " +
            std::to_string(num_keys) + " keyboard keys to track");
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

        // Process non-keyboard SDL events (like QUIT)
        process_sdl_events();

        // Process keyboard state
        process_keyboard_state();
    }

    void Input_Stage::process_sdl_events()
    {
        SDL_Event event;
        int event_count = 0;

        // Poll all non-keyboard SDL events
        while (SDL_PollEvent(&event))
        {
            event_count++;

            switch (event.type)
            {
            case SDL_EVENT_QUIT:
            {
                internal::debug_log("Input_Stage::process_sdl_events", "QUIT event received");
                scene.stop();
                break;
            }

            default:
            {
                // Log other event types for debugging (limit to avoid spam)
                if (event_count <= 3)
                {
                    std::stringstream msg;
                    msg << "Other SDL event type: " << event.type;
                    internal::debug_log("Input_Stage::process_sdl_events", msg.str());
                }
                break;
            }
            }
        }

        if (event_count > 0)
        {
            std::stringstream msg;
            msg << "Processed " << event_count << " non-keyboard SDL events";
            internal::debug_log("Input_Stage::process_sdl_events", msg.str());
        }
    }

    void Input_Stage::process_keyboard_state()
    {
        // Get current keyboard state
        current_key_state = SDL_GetKeyboardState(NULL);

        int key_change_count = 0;

        // Check for changes in key states
        for (int i = 0; i < num_keys; i++)
        {
            // Only process scancodes we care about (convert to our Key_Code enum)
            Key_Code key_code = scancode_to_key_code(static_cast<SDL_Scancode>(i));
            if (key_code == UNDEFINED)
                continue;

            // Key was just pressed (not pressed before, pressed now)
            if (previous_key_state[i] == 0 && current_key_state[i] != 0)
            {
                key_change_count++;

                std::stringstream msg;
                msg << "KEY_DOWN - code: " << key_code;
                internal::debug_log("Input_Stage::process_keyboard_state", msg.str());

                scene.get_input_event_queue().push(
                    key_events.push(key_code, Key_Event::PRESSED)
                );
            }
            // Key was just released (pressed before, not pressed now)
            else if (previous_key_state[i] != 0 && current_key_state[i] == 0)
            {
                key_change_count++;

                std::stringstream msg;
                msg << "KEY_UP - code: " << key_code;
                internal::debug_log("Input_Stage::process_keyboard_state", msg.str());

                scene.get_input_event_queue().push(
                    key_events.push(key_code, Key_Event::RELEASED)
                );
            }

            // Update previous state
            previous_key_state[i] = current_key_state[i];
        }

        if (key_change_count > 0)
        {
            std::stringstream msg;
            msg << "Processed " << key_change_count << " keyboard state changes";
            internal::debug_log("Input_Stage::process_keyboard_state", msg.str());
        }
    }

    bool Input_Stage::is_key_pressed(Key_Code key_code)
    {
        // Convert our Key_Code to SDL scancode
        SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;

        switch (key_code)
        {
        case KEY_A: scancode = SDL_SCANCODE_A; break;
        case KEY_B: scancode = SDL_SCANCODE_B; break;
        case KEY_C: scancode = SDL_SCANCODE_C; break;
        case KEY_D: scancode = SDL_SCANCODE_D; break;
        case KEY_E: scancode = SDL_SCANCODE_E; break;
        case KEY_F: scancode = SDL_SCANCODE_F; break;
        case KEY_G: scancode = SDL_SCANCODE_G; break;
        case KEY_H: scancode = SDL_SCANCODE_H; break;
        case KEY_I: scancode = SDL_SCANCODE_I; break;
        case KEY_J: scancode = SDL_SCANCODE_J; break;
        case KEY_K: scancode = SDL_SCANCODE_K; break;
        case KEY_L: scancode = SDL_SCANCODE_L; break;
        case KEY_M: scancode = SDL_SCANCODE_M; break;
        case KEY_N: scancode = SDL_SCANCODE_N; break;
        case KEY_O: scancode = SDL_SCANCODE_O; break;
        case KEY_P: scancode = SDL_SCANCODE_P; break;
        case KEY_Q: scancode = SDL_SCANCODE_Q; break;
        case KEY_R: scancode = SDL_SCANCODE_R; break;
        case KEY_S: scancode = SDL_SCANCODE_S; break;
        case KEY_T: scancode = SDL_SCANCODE_T; break;
        case KEY_U: scancode = SDL_SCANCODE_U; break;
        case KEY_V: scancode = SDL_SCANCODE_V; break;
        case KEY_W: scancode = SDL_SCANCODE_W; break;
        case KEY_X: scancode = SDL_SCANCODE_X; break;
        case KEY_Y: scancode = SDL_SCANCODE_Y; break;
        case KEY_Z: scancode = SDL_SCANCODE_Z; break;
        case KEY_0: scancode = SDL_SCANCODE_0; break;
        case KEY_1: scancode = SDL_SCANCODE_1; break;
        case KEY_2: scancode = SDL_SCANCODE_2; break;
        case KEY_3: scancode = SDL_SCANCODE_3; break;
        case KEY_4: scancode = SDL_SCANCODE_4; break;
        case KEY_5: scancode = SDL_SCANCODE_5; break;
        case KEY_6: scancode = SDL_SCANCODE_6; break;
        case KEY_7: scancode = SDL_SCANCODE_7; break;
        case KEY_8: scancode = SDL_SCANCODE_8; break;
        case KEY_9: scancode = SDL_SCANCODE_9; break;
        case KEY_LEFT: scancode = SDL_SCANCODE_LEFT; break;
        case KEY_RIGHT: scancode = SDL_SCANCODE_RIGHT; break;
        case KEY_UP: scancode = SDL_SCANCODE_UP; break;
        case KEY_DOWN: scancode = SDL_SCANCODE_DOWN; break;
        default: return false;
        }

        // If current_key_state is somehow not initialized yet
        if (!current_key_state) {
            return false;
        }

        return current_key_state[scancode];
    }

    void Input_Stage::cleanup()
    {
        internal::debug_log("Input_Stage::cleanup", "Starting cleanup");

        scene.get_input_event_queue().clear();
        key_events.clear();

        // Clear keyboard state tracking
        previous_key_state.clear();

        internal::debug_log("Input_Stage::cleanup", "Cleanup completed");
    }

    // Registration code (unchanged from original implementation)
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