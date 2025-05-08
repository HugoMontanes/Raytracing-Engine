/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <engine/Window.hpp>

struct SDL_Window;

namespace udit::engine::internal
{

    struct SDL_Window_Handle : public Window::Handle
    {
        SDL_Window * sdl_window;
    };

}
