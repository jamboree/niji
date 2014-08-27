/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_GRAPHIC_SPIRAL_HPP_INCLUDED
#define BOOST_NIJI_GRAPHIC_SPIRAL_HPP_INCLUDED

#include <boost/niji/support/command.hpp>
#include <boost/niji/support/math/constants.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/point.hpp>

namespace boost { namespace niji
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
        void iterate(Sink& sink) const
        {
            iterate_impl(sink, origin, r / 8, n);
        }
        
        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            iterate(sink);
        }
        
    private:
         
        template<class Sink>
        static void iterate_impl(Sink& sink, point_type const& o, T rr, int n)
        {
            using namespace command;

            T ss = rr * constants::tan_pi_over_8<T>(),
              mm = rr * constants::root2_over_2<T>(),
              r = rr, s = ss, m = mm;

            point_type pt;
            auto q1 = [&](T x, T y)
            {
                pt.reset(x, y);
                r += rr, s += ss, m += mm;
            };
            auto q2 = [&](T x, T y)
            {
                sink(quad_to, o + pt, point_type{o.x + x, o.y + y});
                r += rr, s += ss, m += mm;
            };
            sink(move_to, o);
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
            sink(end_line);
        }
    };
}}

#endif
