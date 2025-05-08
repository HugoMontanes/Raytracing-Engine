/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <memory>
#include <string>

namespace udit::engine
{

    class Window
    {
    public:

        struct Handle
        {
            using Unique_Ptr = std::unique_ptr< Handle >;
        };

    private:

        Handle::Unique_Ptr handle;

    public:

        Window(const std::string & title, unsigned width, unsigned height);
       ~Window();

    public:

        Window(const Window &  ) = delete;
        Window(      Window && ) = delete;

        Window & operator = (const Window &  ) = delete;
        Window & operator = (      Window && ) = delete;

    public:

        unsigned get_width () const;

        unsigned get_height () const;

        Handle * get_handle ()
        {
            return handle.get ();
        }

        void blit_rgb_float (const void * color_buffer, unsigned width, unsigned height);

    };

}
