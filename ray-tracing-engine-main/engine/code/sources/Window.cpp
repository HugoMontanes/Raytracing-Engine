/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <cassert>

#include <engine/Window.hpp>
#include <engine/internal/SDL_Window_Handle.hpp>

#include <SDL3/SDL.h>

namespace udit::engine
{

    Window::Window(const std::string & title, unsigned width, unsigned height)
    {
        // Se hace inicializa SDL:

        if (not SDL_InitSubSystem (SDL_INIT_VIDEO))
        {
            throw "Failed to initialize the video subsystem.";
        }

        auto sdl_window_handle = std::make_unique< internal::SDL_Window_Handle > ();

        // Se crea la ventana activando el soporte para OpenGL:

        sdl_window_handle->sdl_window = SDL_CreateWindow
        (
            title.c_str (),
            int(width ),
            int(height),
            SDL_WINDOW_OPENGL
        );

        assert(sdl_window_handle->sdl_window != nullptr);

        this->handle = std::move (sdl_window_handle);
    }

    Window::~Window()
    {
        if (handle)
        {
            auto sdl_window_handle = static_cast< internal::SDL_Window_Handle * >(handle.get ());

            SDL_DestroyWindow (sdl_window_handle->sdl_window);
        }

        SDL_QuitSubSystem (SDL_INIT_VIDEO);
    }

    unsigned Window::get_width () const
    {
        int width, height;

        SDL_GetWindowSize
        (
            static_cast< internal::SDL_Window_Handle & >(*handle).sdl_window,
            &width,
            &height
        );

        return static_cast< unsigned >(width);
    }

    unsigned Window::get_height () const
    {
        int width, height;

        SDL_GetWindowSize
        (
            static_cast< internal::SDL_Window_Handle & >(*handle).sdl_window,
            &width,
            &height
        );

        return static_cast< unsigned >(height);
    }

    void Window::blit_rgb_float (const void * color_buffer, unsigned buffer_width, unsigned buffer_height)
    {
        SDL_Window * window = static_cast< internal::SDL_Window_Handle & >(*handle).sdl_window;
        void       * buffer =  const_cast< void * >(color_buffer );
        const  int   width  = static_cast< int    >(buffer_width );
        const  int   height = static_cast< int    >(buffer_height);
        const  int   pitch  = sizeof(float) * 3 * width;

        auto source_surface = SDL_CreateSurfaceFrom (width, height, SDL_PIXELFORMAT_RGB96_FLOAT, buffer, pitch);
        auto target_surface = SDL_GetWindowSurface  (window);

        SDL_SetSurfaceBlendMode (source_surface, SDL_BLENDMODE_NONE);
        SDL_BlitSurface         (source_surface, nullptr, target_surface, nullptr);
        SDL_DestroySurface      (source_surface);
    }

}
