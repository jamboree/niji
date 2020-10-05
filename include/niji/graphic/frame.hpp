/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GRAPHIC_FRAME_HPP_INCLUDED
#define NIJI_GRAPHIC_FRAME_HPP_INCLUDED

#include <niji/support/point.hpp>
#include <niji/support/box.hpp>

namespace niji
{
    template<class T>
    struct frame
    {
        using point_type = point<T>;
        using box_type = niji::box<T>;

        box_type box;
        T border;

        frame(box_type const& box, T border)
          : box(box)
          , border(border)
        {}

        template<class Sink>
        void iterate(Sink& sink) const
        {
            iterate_impl(sink, border > 0);
        }

        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            iterate_impl(sink, border < 0);
        }

    private:             
        template<class Sink>
        void iterate_impl(Sink& sink, bool orient) const
        {
            box_type off(box);
            off.offset(border, border);
            if (orient)
            {
                niji::iterate(off, sink);
                niji::reverse_iterate(box, sink);
            }
            else
            {
                niji::iterate(box, sink);
                niji::reverse_iterate(off, sink);
            }
        }
    };
}

#endif