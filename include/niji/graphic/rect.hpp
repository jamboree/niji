/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GRAPHIC_RECT_HPP_INCLUDED
#define NIJI_GRAPHIC_RECT_HPP_INCLUDED

#include <type_traits>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/single_view.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <niji/support/command.hpp>
#include <niji/support/traits.hpp>
#include <niji/support/point.hpp>
#include <niji/support/box.hpp>
#include <niji/support/constants.hpp>

namespace niji { namespace rect_policy
{
    struct round
    {
        template<int n, class Sink, class T, class Rx, class Ry, class Pts>
        static void render
        (
            Sink& sink, T x1, T y1, T x2, T y2
          , Rx const& rx, Ry const& ry, Pts const& x, Pts const& y
        )
        {
            using namespace command;
            using boost::fusion::at_c;
#   if defined(NIJI_NO_CUBIC_APPROX)
            auto mul_tan_pi_over_8 =
                [](T val) { return val * constants::tan_pi_over_8<T>(); };
            auto mul_root2_over_2 =
                [](T val) { return val * constants::root2_over_2<T>(); };

            typename boost::fusion::result_of::as_vector<Rx>::type
                sx(boost::fusion::transform(rx, mul_tan_pi_over_8)),
                sy(boost::fusion::transform(ry, mul_tan_pi_over_8)),
                mx(boost::fusion::transform(rx, mul_root2_over_2)),
                my(boost::fusion::transform(ry, mul_root2_over_2));

            if (!at_c<2 % n>(rx) || !at_c<2 % n>(ry))
                sink(move_to, point<T>{x1, y1});
            else
            {
                sink(move_to, point<T>{x1, at_c<2>(y)});
                sink(quad_to, point<T>{x1, at_c<2>(y) + at_c<2 % n>(sy)}, point<T>{at_c<2>(x) + at_c<2 % n>(mx), at_c<2>(y) + at_c<2 % n>(my)});
                sink(quad_to, point<T>{at_c<2>(x) + at_c<2 % n>(sx), y1}, point<T>{at_c<2>(x), y1});
            }
            if (!at_c<3 % n>(rx) || !at_c<3 % n>(ry))
                sink(line_to, point<T>{x2, y1});
            else
            {
                sink(line_to, point<T>{at_c<3>(x), y1});
                sink(quad_to, point<T>{at_c<3>(x) - at_c<3 % n>(sx), y1}, point<T>{at_c<3>(x) - at_c<3 % n>(mx), at_c<3>(y) + at_c<3 % n>(my)});
                sink(quad_to, point<T>{x2, at_c<3>(y) + at_c<3 % n>(sy)}, point<T>{x2, at_c<3>(y)});
            }
            if (!at_c<0>(rx) || !at_c<0>(ry))
                sink(line_to, point<T>{x2, y2});
            else
            {
                sink(line_to, point<T>{x2, at_c<0>(y)});
                sink(quad_to, point<T>{x2, at_c<0>(y) - at_c<0>(sy)}, point<T>{at_c<0>(x) - at_c<0>(mx), at_c<0>(y) - at_c<0>(my)});
                sink(quad_to, point<T>{at_c<0>(x) - at_c<0>(sx), y2}, point<T>{at_c<0>(x), y2});
            }
            if (!at_c<1 % n>(rx) || !at_c<1 % n>(ry))
                sink(line_to, point<T>{x1, y2});
            else
            {
                sink(line_to, point<T>{at_c<1>(x), y2});
                sink(quad_to, point<T>{at_c<1>(x) + at_c<1 % n>(sx), y2}, point<T>{at_c<1>(x) + at_c<1 % n>(mx), at_c<1>(y) - at_c<1 % n>(my)});
                sink(quad_to, point<T>{x1, at_c<1>(y) - at_c<1 % n>(sy)}, point<T>{x1, at_c<1>(y)});
            }
#   else
            auto mul_cubic_arc_factor =
                [](T val) { return val * constants::cubic_arc_factor<T>(); };
            
            typename boost::fusion::result_of::as_vector<Rx>::type
                sx(boost::fusion::transform(rx, mul_cubic_arc_factor)),
                sy(boost::fusion::transform(ry, mul_cubic_arc_factor));

            if (!at_c<2 % n>(rx) || !at_c<2 % n>(ry))
                sink(move_to, point<T>{x1, y1});
            else
            {
                sink(move_to, point<T>{x1, at_c<2>(y)});
                sink(cubic_to, point<T>{x1, at_c<2>(y) + at_c<2 % n>(sy)}, point<T>{at_c<2>(x) + at_c<2 % n>(sx), y1}, point<T>{at_c<2>(x), y1});
            }
            if (!at_c<3 % n>(rx) || !at_c<3 % n>(ry))
                sink(line_to, point<T>{x2, y1});
            else
            {
                sink(line_to, point<T>{at_c<3>(x), y1});
                sink(cubic_to, point<T>{at_c<3>(x) - at_c<3 % n>(sx), y1}, point<T>{x2, at_c<3>(y) + at_c<3 % n>(sy)}, point<T>{x2, at_c<3>(y)});
            }
            if (!at_c<0>(rx) || !at_c<0>(ry))
                sink(line_to, point<T>{x2, y2});
            else
            {
                sink(line_to, point<T>{x2, at_c<0>(y)});
                sink(cubic_to, point<T>{x2, at_c<0>(y) - at_c<0>(sy)}, point<T>{at_c<0>(x) - at_c<0>(sx), y2}, point<T>{at_c<0>(x), y2});
            }
            if (!at_c<1 % n>(rx) || !at_c<1 % n>(ry))
                sink(line_to, point<T>{x1, y2});
            else
            {
                sink(line_to, point<T>{at_c<1>(x), y2});
                sink(cubic_to, point<T>{at_c<1>(x) + at_c<1 % n>(sx), y2}, point<T>{x1, at_c<1>(y) - at_c<1 % n>(sy)}, point<T>{x1, at_c<1>(y)});
            }
#   endif
            sink(end_closed);
        }
    };
    
    struct bevel
    {
        template<int n, class Sink, class T, class Rx, class Ry, class Pts>
        static void render
        (
            Sink& sink, T x1, T y1, T x2, T y2
          , Rx const& rx, Ry const& ry, Pts const& x, Pts const& y
        )
        {
            using namespace command;
            using boost::fusion::at_c;

            if (!at_c<2 % n>(rx) || !at_c<2 % n>(ry))
                sink(move_to, point<T>{x1, y1});
            else
            {
                sink(move_to, point<T>{x1, at_c<2>(y)});
                sink(line_to, point<T>{at_c<2>(x), y1});
            }
            if (!at_c<3 % n>(rx) || !at_c<3 % n>(ry))
                sink(line_to, point<T>{x2, y1});
            else
            {
                sink(line_to, point<T>{at_c<3>(x), y1});
                sink(line_to, point<T>{x2, at_c<3>(y)});
            }
            if (!at_c<0>(rx) || !at_c<0>(ry))
                sink(line_to, point<T>{x2, y2});
            else
            {
                sink(line_to, point<T>{x2, at_c<0>(y)});
                sink(line_to, point<T>{at_c<0>(x), y2});
            }
            if (!at_c<1 % n>(rx) || !at_c<1 % n>(ry))
                sink(line_to, point<T>{x1, y2});
            else
            {
                sink(line_to, point<T>{at_c<1>(x), y2});
                sink(line_to, point<T>{x1, at_c<1>(y)});
            }
            sink(end_closed);
        }
    };
}}

namespace niji
{
    template<class T, class Policy, class Radius = T>
    struct rect
    {
        using point_type = point<T>;
        using box_type = niji::box<point_type>;

        using view_t = std::conditional_t<
            boost::fusion::traits::is_sequence<Radius>::value
          , Radius const&, boost::fusion::single_view<Radius>>;

        box_type box;
        Radius rx, ry;

        rect(box_type const& box, Radius const& r)
          : box(box)
          , rx(r), ry(r)
        {}

        rect(box_type const& box, Radius const& rx, Radius const& ry)
          : box(box)
          , rx(rx), ry(ry)
        {}

        rect(point_type const& min, point_type const& max, Radius const& r)
          : box(min, max)
          , rx(r), ry(r)
        {}

        rect(point_type const& min, point_type const& max, Radius const& rx, Radius const& ry)
          : box(min, max)
          , rx(rx), ry(ry)
        {}

        void radius(Radius const& r)
        {
            rx = r;
            ry = r;
        }
        
        void radius(Radius const& x, Radius const& y)
        {
            rx = x;
            ry = y;
        }

        template<class Sink>
        void render(Sink& sink) const
        {
            point_type const& min(box.min_corner);
            point_type const& max(box.max_corner);
            render_impl(sink, max.x, max.y, min.x, min.y, view_t(rx), view_t(ry));
        }
        
        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            point_type const& min(box.min_corner);
            point_type const& max(box.max_corner);
            render_impl(sink, max.x, min.y, min.x, max.y, view_t(rx),
                boost::fusion::as_vector(boost::fusion::transform(view_t(ry), std::negate<T>())));
        }
        
    private:
         
        template<class Sink, class Rx, class Ry>
        static void render_impl
        (
            Sink& sink, T x1, T y1, T x2, T y2, Rx const& rx, Ry const& ry
        )
        {
            using boost::fusion::at_c;

            //     x2                             x1
            //  y1 +-------+--------------+-------+ y1
            //     |   3   |       |      |   2   |
            //     +-------+       |      +-------+
			//     |               |              |
            //     |---------------o--------------|
            //     |               |              |
            //     +-------+       |      +-------+
            //     |   0   |       |      |   1   |
            //  y2 +-------+--------------+-------+ y2
            //     x2                            x1

            constexpr int n = boost::fusion::result_of::size<Rx>::value;
            boost::fusion::vector<T, T, T, T>
                x(x2 + at_c<0>(rx), x1 - at_c<1 % n>(rx), x1 - at_c<2 % n>(rx), x2 + at_c<3 % n>(rx)),
                y(y2 + at_c<0>(ry), y2 + at_c<1 % n>(ry), y1 - at_c<2 % n>(ry), y1 - at_c<3 % n>(ry));
            
            Policy::template render<n>(sink, x1, y1, x2,  y2, rx, ry, x, y);
        }
    };
    
    template<class T, class Radius = T>
    using round_rect = rect<T, rect_policy::round, Radius>;
    
    template<class T, class Radius = T>
    using bevel_rect = rect<T, rect_policy::bevel, Radius>;
}

#endif