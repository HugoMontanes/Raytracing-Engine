/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <algorithm>
#include <vector>

namespace udit::raytracer
{

    template< typename TYPE >
    class Buffer
    {
    public:

        using Value_Type = TYPE;

    private:

        unsigned width;
        unsigned height;

        std::vector< Value_Type > elements;

    public:

        Buffer()
        {
            width = height = 0;
        }

        Buffer(unsigned given_width, unsigned given_height)
        :
            width   (given_width   ),
            height  (given_height  ),
            elements(width * height)
        {
        }

    public:

        unsigned size () const
        {
            return static_cast< unsigned >(elements.size ());
        }

        bool empty () const
        {
            return elements.empty ();
        }

        unsigned get_width () const
        {
            return width;
        }

        unsigned get_height () const
        {
            return height;
        }

        Value_Type * data ()
        {
            return elements.data ();
        }

        const Value_Type * data () const
        {
            return elements.data ();
        }

    public:

        template< class BUFFER >
        void resize_as (const BUFFER & other)
        {
            this->resize (other.get_width (), other.get_height ());
        }

        void resize (unsigned new_width, unsigned new_height)
        {
            if (width != new_width || height != new_height)
            {
                elements.resize ((width = new_width) * (height = new_height));
            }
        }

    public:

        void clear (const Value_Type & value)
        {
            std::fill_n (elements.begin (), elements.size (), value);
        }

        const Value_Type & get (unsigned x, unsigned y) const
        {
            return elements[y * width + x];
        }

        void set (unsigned x, unsigned y, const Value_Type & new_value)
        {
            elements[y * width + x] = new_value;
        }

        const Value_Type & get (unsigned offset) const
        {
            return elements[offset];
        }

        void set (unsigned offset, const Value_Type & new_value)
        {
            elements[offset] = new_value;
        }

        Value_Type & operator [] (unsigned offset)
        {
            return elements[offset];
        }

        const Value_Type & operator [] (unsigned offset) const
        {
            return elements[offset];
        }

    };

}
