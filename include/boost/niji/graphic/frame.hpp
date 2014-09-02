/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_GRAPHIC_FRAME_HPP_INCLUDED
#define BOOST_NIJI_GRAPHIC_FRAME_HPP_INCLUDED

#include <boost/niji/iterate.hpp>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/point.hpp>
#include <boost/niji/support/box.hpp>

namespace boost { namespace niji
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
}}

#endif
