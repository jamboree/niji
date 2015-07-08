/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GRAPHIC_CROSS_HPP_INCLUDED
#define NIJI_GRAPHIC_CROSS_HPP_INCLUDED

#include <niji/render.hpp>
#include <niji/support/command.hpp>
#include <niji/support/point.hpp>

namespace niji
{
    template<class T>
    struct cross
    {
        using point_type = point<T>;

        point_type center;
        T r;

        cross(point_type const& center, T r) : center(center), r(r) {}

        template<class Sink>
        void render(Sink& sink) const
        {
            auto line = [&](vector<T> v)
            {
                using namespace command;
                sink(move_to, center - v);
                sink(line_to, center + v);
                sink(end_line);
            };
            line({r, 0});
            line({0, r});
        }

        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            render(sink);
        }
    };
}

#endif