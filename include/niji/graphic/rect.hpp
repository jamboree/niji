/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GRAPHIC_RECT_HPP_INCLUDED
#define NIJI_GRAPHIC_RECT_HPP_INCLUDED

#include <type_traits>
#include <niji/support/point.hpp>
#include <niji/support/box.hpp>
#include <niji/support/numbers.hpp>

namespace niji::rect_corner
{
    struct round
    {
        template<class Sink, class T>
        static void iterate
        (
            Sink& sink, T x1, T y1, T x2, T y2
          , point<T> const (&r)[4], point<T> const (&pt)[4]
        )
        {
#   if defined(NIJI_NO_CUBIC_APPROX)
            const auto mul_tan_pi_over_8 = [](point<T> pt)
            {
                return point<T>(pt.x * numbers::tan_pi_over_8<T>, pt.y * numbers::tan_pi_over_8<T>);
            };

            const point<T> s[4]
            {
                mul_tan_pi_over_8(r[0]),
                mul_tan_pi_over_8(r[1]),
                mul_tan_pi_over_8(r[2]),
                mul_tan_pi_over_8(r[3])
            };

            const auto mul_root2_over_2 = [](point<T> pt)
            {
                return point<T>(pt.x * numbers::root2_over_2<T>, pt.y * numbers::root2_over_2<T>);
            };

            const point<T> m[4]
            {
                mul_root2_over_2(r[0]),
                mul_root2_over_2(r[1]),
                mul_root2_over_2(r[2]),
                mul_root2_over_2(r[3])
            };

            if (!r[2].x || !r[2].y)
                sink.move_to(point<T>{x1, y1});
            else
            {
                sink.move_to(point<T>{x1, pt[2].y});
                sink.quad_to(point<T>{x1, pt[2].y + s[2].y}, point<T>{pt[2].x + m[2].x, pt[2].y + m[2].y});
                sink.quad_to(point<T>{pt[2].x + s[2].x, y1}, point<T>{pt[2].x, y1});
            }
            if (!r[3].x || !r[3].y)
                sink.line_to(point<T>{x2, y1});
            else
            {
                sink.line_to(point<T>{pt[3].x, y1});
                sink.quad_to(point<T>{pt[3].x - s[3].x, y1}, point<T>{pt[3].x - m[3].x, pt[3].y + m[3].y});
                sink.quad_to(point<T>{x2, pt[3].y + s[3].y}, point<T>{x2, pt[3].y});
            }
            if (!r[0].x || !r[0].y)
                sink.line_to(point<T>{x2, y2});
            else
            {
                sink.line_to(point<T>{x2, pt[0].y});
                sink.quad_to(point<T>{x2, pt[0].y - s[0].y}, point<T>{pt[0].x - m[0].x, pt[0].y - m[0].y});
                sink.quad_to(point<T>{pt[0].x - s[0].x, y2}, point<T>{pt[0].x, y2});
            }
            if (!r[1].x || !r[1].y)
                sink.line_to(point<T>{x1, y2});
            else
            {
                sink.line_to(point<T>{pt[1].x, y2});
                sink.quad_to(point<T>{pt[1].x + s[1].x, y2}, point<T>{pt[1].x + m[1].x, pt[1].y - m[1].y});
                sink.quad_to(point<T>{x1, pt[1].y - s[1].y}, point<T>{x1, pt[1].y});
            }
#   else
            const auto mul_cubic_arc_factor = [](point<T> pt)
            {
                return point<T>(pt.x * numbers::cubic_arc_factor<T>, pt.y * numbers::cubic_arc_factor<T>);
            };
            
            const point<T> s[4]
            {
                mul_cubic_arc_factor(r[0]),
                mul_cubic_arc_factor(r[1]),
                mul_cubic_arc_factor(r[2]),
                mul_cubic_arc_factor(r[3])
            };

            if (!r[2].x || !r[2].y)
                sink.move_to(point<T>{x1, y1});
            else
            {
                sink.move_to(point<T>{x1, pt[2].y});
                sink.cubic_to(point<T>{x1, pt[2].y + s[2].y}, point<T>{pt[2].x + s[2].x, y1}, point<T>{pt[2].x, y1});
            }
            if (!r[3].x || !r[3].y)
                sink.line_to(point<T>{x2, y1});
            else
            {
                sink.line_to(point<T>{pt[3].x, y1});
                sink.cubic_to(point<T>{pt[3].x - s[3].x, y1}, point<T>{x2, pt[3].y + s[3].y}, point<T>{x2, pt[3].y});
            }
            if (!r[0].x || !r[0].y)
                sink.line_to(point<T>{x2, y2});
            else
            {
                sink.line_to(point<T>{x2, pt[0].y});
                sink.cubic_to(point<T>{x2, pt[0].y - s[0].y}, point<T>{pt[0].x - s[0].x, y2}, point<T>{pt[0].x, y2});
            }
            if (!r[1].x || !r[1].y)
                sink.line_to(point<T>{x1, y2});
            else
            {
                sink.line_to(point<T>{pt[1].x, y2});
                sink.cubic_to(point<T>{pt[1].x + s[1].x, y2}, point<T>{x1, pt[1].y - s[1].y}, point<T>{x1, pt[1].y});
            }
#   endif
            sink.end_closed();
        }
    };
    
    struct bevel
    {
        template<class Sink, class T>
        static void iterate
        (
            Sink& sink, T x1, T y1, T x2, T y2
          , point<T> const (&r)[4], point<T> const (&pt)[4]
        )
        {
            if (!r[2].x || !r[2].y)
                sink.move_to(point<T>{x1, y1});
            else
            {
                sink.move_to(point<T>{x1, pt[2].y});
                sink.line_to(point<T>{pt[2].x, y1});
            }
            if (!r[3].x || !r[3].y)
                sink.line_to(point<T>{x2, y1});
            else
            {
                sink.line_to(point<T>{pt[3].x, y1});
                sink.line_to(point<T>{x2, pt[3].y});
            }
            if (!r[0].x || !r[0].y)
                sink.line_to(point<T>{x2, y2});
            else
            {
                sink.line_to(point<T>{x2, pt[0].y});
                sink.line_to(point<T>{pt[0].x, y2});
            }
            if (!r[1].x || !r[1].y)
                sink.line_to(point<T>{x1, y2});
            else
            {
                sink.line_to(point<T>{pt[1].x, y2});
                sink.line_to(point<T>{x1, pt[1].y});
            }
            sink.end_closed();
        }
    };
}

namespace niji
{
    namespace detail
    {
        template<class T, class Base>
        struct negate_y_view
        {
            Base const& base;

            point<T> operator[](std::size_t n) const
            {
                return point<T>(base[n].x, -base[n].y);
            }
        };
    }

    template<class T>
    struct radius1
    {
        T value;

        radius1(T value) : value(value) {}

        point<T> operator[](std::size_t) const
        {
            return point<T>(value, value);
        }
    };

    template<class T, class Policy, class Radius = radius1<T>>
    struct rect
    {
        using point_type = point<T>;
        using box_type = niji::box<T>;

        box_type box;
        Radius radius;

        rect(box_type const& box, Radius const& radius)
          : box(box), radius(radius)
        {}

        rect(point_type const& min, point_type const& max, Radius const& radius)
          : box(min, max), radius(radius)
        {}

        template<class Sink>
        void iterate(Sink& sink) const
        {
            point_type const& min = box.min_corner;
            point_type const& max = box.max_corner;
            iterate_impl(sink, max.x, max.y, min.x, min.y, radius);
        }
        
        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            point_type const& min = box.min_corner;
            point_type const& max = box.max_corner;
            iterate_impl(sink, max.x, min.y, min.x, max.y, detail::negate_y_view<T, Radius>{radius});
        }
        
    private:
         
        template<class Sink, class RadiusView>
        static void iterate_impl
        (
            Sink& sink, T x1, T y1, T x2, T y2, RadiusView const& radius
        )
        {
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
            point<T> r[4]{radius[0], radius[1], radius[2], radius[3]};
            point<T> pt[4]
            {
                point<T>(x2 + r[0].x, y2 + r[0].y),
                point<T>(x1 - r[1].x, y2 + r[1].y),
                point<T>(x1 - r[2].x, y1 - r[2].y),
                point<T>(x2 + r[3].x, y1 - r[3].y)
            };
            Policy::iterate(sink, x1, y1, x2,  y2, r, pt);
        }
    };
    
    template<class T, class Radius = radius1<T>>
    using round_rect = rect<T, rect_corner::round, Radius>;
    
    template<class T, class Radius = radius1<T>>
    using bevel_rect = rect<T, rect_corner::bevel, Radius>;
}

#endif