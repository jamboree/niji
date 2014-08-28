/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_GRAPHIC_RECT_HPP_INCLUDED
#define BOOST_NIJI_GRAPHIC_RECT_HPP_INCLUDED

#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/single_view.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/math/constants.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/point.hpp>
#include <boost/niji/support/box.hpp>

namespace boost { namespace niji { namespace rect_policy
{
    struct round
    {
        template<int n, class Sink, class T, class Rx, class Ry, class Pts>
        static void iterate
        (
            Sink& sink, T x1, T y1, T x2, T y2
          , Rx const& rx, Ry const& ry, Pts const& x, Pts const& y
        )
        {
            using namespace command;
            using fusion::at_c;
#   if defined(BOOST_NIJI_NO_CUBIC_APPROX)
            auto mul_tan_pi_over_8 =
                [](T val) { return val * constants::tan_pi_over_8<T>(); };
            auto mul_root2_over_2 =
                [](T val) { return val * constants::root2_over_2<T>(); };

            typename fusion::result_of::as_vector<Rx>::type
                sx(fusion::transform(rx, mul_tan_pi_over_8)),
                sy(fusion::transform(ry, mul_tan_pi_over_8)),
                mx(fusion::transform(rx, mul_root2_over_2)),
                my(fusion::transform(ry, mul_root2_over_2));

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
            
            typename fusion::result_of::as_vector<Rx>::type
                sx(fusion::transform(rx, mul_cubic_arc_factor)),
                sy(fusion::transform(ry, mul_cubic_arc_factor));

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
            sink(end_poly);
        }
    };
    
    struct bevel
    {
        template<int n, class Sink, class T, class Rx, class Ry, class Pts>
        static void iterate
        (
            Sink& sink, T x1, T y1, T x2, T y2
          , Rx const& rx, Ry const& ry, Pts const& x, Pts const& y
        )
        {
            using namespace command;
            using fusion::at_c;

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
            sink(end_poly);
        }
    };
}}}

namespace boost { namespace niji
{
    template<class T, class Policy, class Radius = T>
    struct rect
    {
        using point_type = point<T>;
        using box_type = niji::box<point_type>;

        using view_t = typename
            mpl::if_<fusion::traits::is_sequence<Radius>,
                Radius const&, fusion::single_view<Radius>>::type;

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
        void iterate(Sink& sink) const
        {
            point_type const& min(box.min_corner);
            point_type const& max(box.max_corner);
            iterate_impl(sink, max.x, max.y, min.x, min.y, view_t(rx), view_t(ry));
        }
        
        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            point_type const& min(box.min_corner);
            point_type const& max(box.max_corner);
            iterate_impl(sink, max.x, min.y, min.x, max.y, view_t(rx),
                fusion::as_vector(fusion::transform(view_t(ry), std::negate<T>())));
        }
        
    private:
         
        template<class Sink, class Rx, class Ry>
        static void iterate_impl
        (
            Sink& sink, T x1, T y1, T x2, T y2, Rx const& rx, Ry const& ry
        )
        {
            using fusion::at_c;

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

            constexpr int n = fusion::result_of::size<Rx>::value;
            fusion::vector<T, T, T, T>
                x(x2 + at_c<0>(rx), x1 - at_c<1 % n>(rx), x1 - at_c<2 % n>(rx), x2 + at_c<3 % n>(rx)),
                y(y2 + at_c<0>(ry), y2 + at_c<1 % n>(ry), y1 - at_c<2 % n>(ry), y1 - at_c<3 % n>(ry));
            
            Policy::template iterate<n>(sink, x1, y1, x2,  y2, rx, ry, x, y);
        }
    };
    
    template<class T, class Radius = T>
    using round_rect = rect<T, rect_policy::round, Radius>;
    
    template<class T, class Radius = T>
    using bevel_rect = rect<T, rect_policy::bevel, Radius>;
}}

#endif
