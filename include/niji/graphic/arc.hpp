/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GRAPHIC_ARC_HPP_INCLUDED
#define NIJI_GRAPHIC_ARC_HPP_INCLUDED

#include <niji/support/point.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/box.hpp>
#include <niji/support/bezier.hpp>

namespace niji
{
    template<class T>
    struct arc
    {
        using point_type = point<T>;
        using box_type = niji::box<T>;
        
        box_type box;
        T start, sweep;

        arc(box_type const& box, T start, T sweep)
          : box(box), start(start), sweep(sweep)
        {}
        
        template<class Sink>
        void iterate(Sink& sink) const
        {
            iterate_impl(sink, start, start + sweep);
        }
        
        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            iterate_impl(sink, start + sweep, start);
        }
        
    private:
        template<class Sink>
        void iterate_impl(Sink& sink, T start, T stop) const
        {
            using std::sin;
            using std::cos;

            transforms::affine<T> affine;
            T rx = box.width() / 2, ry = box.height() / 2;
            affine.scale(rx, ry).translate(box.min_corner.x + rx, box.min_corner.y + ry);

            vector<T> u(cos(start), sin(start)), v(cos(stop), sin(stop));
#   if defined(NIJI_NO_CUBIC_APPROX)
            point<T> pts[17];
            auto it = pts, end = bezier::build_quad_arc(u, v, stop > start, &affine, pts);
            sink.move_to(*it++);
            for ( ; it != end; it += 2)
                sink.quad_to(*it, it[1]);
#   else
            point<T> pts[13];
            auto it = pts, end = bezier::build_cubic_arc(u, v, stop > start, &affine, pts);
            sink.move_to(*it++);
            for ( ; it != end; it += 3)
                sink.cubic_to(*it, it[1], it[2]);
#   endif
            sink.end_open();
        }
    };
}

#endif