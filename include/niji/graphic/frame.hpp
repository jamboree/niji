/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GRAPHIC_FRAME_HPP_INCLUDED
#define NIJI_GRAPHIC_FRAME_HPP_INCLUDED

#include <niji/render.hpp>
#include <niji/support/command.hpp>
#include <niji/support/point.hpp>
#include <niji/support/box.hpp>
#include <niji/geometry/box.hpp>

namespace niji
{
    template<class T>
    struct frame
    {
        using point_type = point<T>;
        using box_type = niji::box<point_type>;

        box_type box;
        T border;

        frame(box_type const& box, T border)
          : box(box)
          , border(border)
        {}

        frame(point_type const& min, point_type const& max, T border)
          : box(min, max)
          , border(border)
        {}

        template<class Sink>
        void render(Sink& sink) const
        {
            render_impl(sink, border > 0);
        }

        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            render_impl(sink, border < 0);
        }

    private:
                        
        template<class Sink>
        void render_impl(Sink& sink, bool orient) const
        {
            box_type off(box);
            off.offset(border, border);
            if (orient)
            {
                niji::render(off, sink);
                niji::inverse_render(box, sink);
            }
            else
            {
                niji::render(box, sink);
                niji::inverse_render(off, sink);
            }
        }
    };
}

#endif