/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GRAPHIC_CROSS_HPP_INCLUDED
#define NIJI_GRAPHIC_CROSS_HPP_INCLUDED

#include <niji/support/point.hpp>

namespace niji
{
    template<class T>
    struct cross
    {
        using point_type = point<T>;

        point_type center;
        T radius;

        cross(point_type const& center, T radius) : center(center), radius(radius) {}

        template<class Sink>
        void iterate(Sink& sink) const
        {
            auto line = [&](vector<T> v)
            {
                sink.move_to(center - v);
                sink.line_to(center + v);
                sink.end_open();
            };
            line({radius, 0});
            line({0, radius});
        }

        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            iterate(sink);
        }
    };
}

#endif