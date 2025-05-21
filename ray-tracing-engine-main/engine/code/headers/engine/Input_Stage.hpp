#pragma once

#include <engine/Key_Event.hpp>
#include <engine/Stage.hpp>
#include <engine/Timer.hpp>
#include <iostream>
#include <atomic>
#include <future>
#include <vector>

// Add this include for SDL types
#include <SDL3/SDL.h>

namespace udit::engine
{
    class Input_Stage : public Stage
    {
        using Key_Event_Pool = Input_Event::Queue_Pool< Key_Event >;

    private:
        Key_Event_Pool key_events;

        // Keyboard state tracking - now SDL types will be recognized
        const bool* current_key_state;
        std::vector<bool> previous_key_state;
        int num_keys; // Total number of keys to track

        // Convert SDL scancode to our Key_Code enum
        Key_Code scancode_to_key_code(SDL_Scancode scancode);

        // Process keyboard state changes
        void process_keyboard_state();

        // Process other SDL events
        void process_sdl_events();

    public:
        Input_Stage(Scene& scene)
            : Stage(scene),
            current_key_state(nullptr),
            num_keys(0)
        {}

        void prepare() override;
        void compute(float delta_time) override;
        void cleanup() override;

        // Direct access to check if a key is currently pressed
        bool is_key_pressed(Key_Code key_code);
    };
}