/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GRAPHIC_SPIRAL_HPP_INCLUDED
#define NIJI_GRAPHIC_SPIRAL_HPP_INCLUDED

#include <niji/support/command.hpp>
#include <niji/support/traits.hpp>
#include <niji/support/point.hpp>
#include <niji/support/math/constants.hpp>

namespace niji
{
    template<class T>
    struct spiral
    {
        using point_type = point<T>;
        
        point_type origin;
        T r;
        int n;

        spiral(point_type const& pt, T r, int n)
          : origin(pt), r(r), n(n)
        {}

        template<class Sink>
        void render(Sink& sink) const
        {
            render_impl(sink, origin, r, n);
        }
        
        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            render(sink);
        }
        
    private:
         
        template<class Sink>
        static void render_impl(Sink& sink, point_type const& o, T r, int n)
        {
            using namespace command;

            sink(move_to, o);
#   if defined(NIJI_NO_CUBIC_APPROX)
            r /= 16;
            
            T ds = r * constants::tan_pi_over_8<T>(),
              dm = r * constants::root2_over_2<T>(),
              dr = r, s = ds, m = dm;

            point_type pt;
            auto q1 = [&](T x, T y)
            {
                pt.reset(x, y);
                r += dr, s += ds, m += dm;
            };
            auto q2 = [&](T x, T y)
            {
                sink(quad_to, o + pt, point_type{o.x + x, o.y + y});
                r += dr, s += ds, m += dm;
            };
            for (int i = 0; i != n; ++i)
            {
                q1(r, s), q2(m, m);
                q1(s, r), q2(0, r);
                q1(-s, r), q2(-m, m);
                q1(-r, s), q2(-r, 0);
                q1(-r, -s), q2(-m, -m);
                q1(-s, -r), q2(0, -r);
                q1(s, -r), q2(m, -m);
                q1(r, -s), q2(r, 0);
            }
#   else
            r /= 12; // the angle is near pi/6
            
            T ds = r * constants::cubic_arc_factor<T>(),
              dr = r, s = ds;

            point_type pt1, pt2;
            auto c1 = [&](T x, T y)
            {
                pt1.reset(x, y);
                r += dr, s += ds;
            };
            auto c2 = [&](T x, T y)
            {
                pt2.reset(x, y);
                r += dr, s += ds;
            };
            auto c3 = [&](T x, T y)
            {
                sink(cubic_to, o + pt1, o + pt2, point_type{o.x + x, o.y + y});
                r += dr, s += ds;
            };
            for (int i = 0; i != n; ++i)
            {
                c1(r, s), c2(s, r), c3(0, r);
                c1(-s, r), c2(-r, s), c3(-r, 0);
                c1(-r, -s), c2(-s, -r), c3(0, -r);
                c1(s, -r), c2(r, -s), c3(r, 0);
            }
#   endif
            sink(end_line);
        }
    };
}

#endif
