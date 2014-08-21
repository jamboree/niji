/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_VIEW_STROKE_CAP_STYLE_HPP_INCLUDED
#define BOOST_NIJI_VIEW_STROKE_CAP_STYLE_HPP_INCLUDED

#include <functional>
#include <boost/niji/path.hpp>
#include <boost/niji/support/point.hpp>
#include <boost/niji/support/vector.hpp>
#include <boost/niji/support/bezier/basics.hpp>

namespace boost { namespace niji { namespace detail
{
    template<class T>
    using cap_style_fn = std::function<void(
            path<point<T>>&, point<T> const&, vector<T> const&, bool)>;
}}}

namespace boost { namespace niji { namespace cap_styles
{
    struct butt
    {
        template<class T>
        void operator()
        (
            path<point<T>>& path, point<T> const& pt
          , vector<T> const& normal, bool is_line
        ) const
        {
            path.join(pt + normal);
            path.join(pt - normal);
        }
    };

    struct square
    {
        template<class T>
        void operator()
        (
            path<point<T>>& path, point<T> pt
          , vector<T> const& normal, bool is_line
        ) const
        {
            pt += vectors::normal_ccw(normal);
            path.join(pt + normal);
            path.join(pt - normal);
        }
    };

    struct round
    {
        template<class T>
        struct rot
        {
            T const ca, sa, x0, y0;
            
            point<T> operator()(T x, T y) const
            {
                return point<T>(ca * x - y * sa + x0, ca * y + sa * x + y0);
            }
        };

        template<class T>
        void operator()
        (
            path<point<T>>& path, point<T> const& pt
          , vector<T> const& normal, bool is_line
        ) const
        {
#if 1
            vector<T> v(vectors::normal_ccw(normal))
                    , s(normal * constants::cubic_arc_factor<T>())
                    , sn(vectors::normal_ccw(s));
            point<T> pt1(pt + normal), pt2(pt + v), pt3(pt - normal);
            path.join(pt1);
            path.unsafe_cubic_to(pt1 + sn, pt2 + s, pt2);
            path.unsafe_cubic_to(pt2 - s, pt3 + sn, pt3);
#else
            T const s = constants::tan_pi_over_8<T>(),
                    m = constants::root2_over_2<T>();

            rot<T> f = {normal.x, normal.y, pt.x, pt.y};
            path.join(pt + normal);
            path.unsafe_quad_to(f(1, s), f(m, m));
            path.unsafe_quad_to(f(s, 1), f(0, 1));
            path.unsafe_quad_to(f(-s, 1), f(-m, m));
            path.unsafe_quad_to(f(-1, s), f(-1, 0));
#endif
        }
    };
}}}

namespace boost { namespace niji
{
    template<class T>
    struct cap_style : detail::cap_style_fn<T>
    {
        template<class Capper = cap_styles::butt>
        cap_style(Capper capper = {})
          : detail::cap_style_fn<T>(capper)
        {}
    };
}}

#endif
