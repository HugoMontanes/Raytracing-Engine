/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <engine/Display_Stage.hpp>
#include <engine/Scene.hpp>
#include <engine/Window.hpp>

#include <engine/internal/SDL_Window_Handle.hpp>

#include <SDL3/SDL.h>

namespace udit::engine
{

    template<>
    Stage::Unique_Ptr Stage::create< Display_Stage > (Scene & scene)
    {
        return std::make_unique< Display_Stage > (scene);
    }

    template<>
    template<>
    Id Registrar< Stage >::record< Display_Stage > ()
    {
        return registry ().add ("Display_Stage", Stage::create< Display_Stage >);
    }

    template<>
    Id Stage::setup< Display_Stage > ()
    {
        static Id id = INVALID_ID;

        if (not_valid (id))
        {
            id = Stage::record< Display_Stage > ();
        }

        return id;
    }

    template<>
    Id Stage::id_of< Display_Stage > ()
    {
        return Stage::setup< Display_Stage > ();
    }

    void Display_Stage::compute (float)
    {
        SDL_UpdateWindowSurface
        (
            static_cast< internal::SDL_Window_Handle * >
            (
                scene.get_window ().get_handle ()
            )
            ->sdl_window
        );
    }

}
