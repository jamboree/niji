/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GRAPHIC_ELLIPSE_HPP_INCLUDED
#define NIJI_GRAPHIC_ELLIPSE_HPP_INCLUDED

#include <niji/support/command.hpp>
#include <niji/support/traits.hpp>
#include <niji/support/point.hpp>
#include <niji/support/math/constants.hpp>

namespace niji
{
    template<class T>
    struct ellipse
    {
        using point_type = point<T>;
        
        point_type origin;
        T rx, ry;

        ellipse(point_type const& pt, T r)
          : origin(pt), rx(r), ry(r)
        {}

        ellipse(point_type const& pt, T rx, T ry)
          : origin(pt), rx(rx), ry(ry)
        {}
        
        std::pair<T, T> radius() const
        {
            return std::pair<T, T>(rx, ry);
        }

        void radius(T r)
        {
            rx = r;
            ry = r;
        }

        void radius(T x, T y)
        {
            rx = x;
            ry = y;
        }
        
        template<class Sink>
        void render(Sink& sink) const
        {
            render_impl(sink, ry);
        }
        
        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            render_impl(sink, -ry);
        }
        
    private:
         
        template<class Sink>
        void render_impl(Sink& sink, T ry) const
        {
            using namespace command;

            T x = origin.x, y = origin.y,
              x1 = x + rx, x2 = x - rx,
              y1 = y + ry, y2 = y - ry;
            
            sink(move_to, point_type{x1, y});
#   if defined(NIJI_NO_CUBIC_APPROX)
            T sx = rx * constants::tan_pi_over_8<T>(),
              sy = ry * constants::tan_pi_over_8<T>(),
              mx = rx * constants::root2_over_2<T>(),
              my = ry * constants::root2_over_2<T>(),
              x3 = x + sx, x4 = x - sx, x5 = x + mx, x6 = x - mx,
              y3 = y + sy, y4 = y - sy, y5 = y + my, y6 = y - my;

            sink(quad_to, point_type{x1, y3}, point_type{x5, y5});
            sink(quad_to, point_type{x3, y1}, point_type{x, y1});
            sink(quad_to, point_type{x4, y1}, point_type{x6, y5});
            sink(quad_to, point_type{x2, y3}, point_type{x2, y});
            sink(quad_to, point_type{x2, y4}, point_type{x6, y6});
            sink(quad_to, point_type{x4, y2}, point_type{x, y2});
            sink(quad_to, point_type{x3, y2}, point_type{x5, y6});
            sink(quad_to, point_type{x1, y4}, point_type{x1, y});
#   else
            T sx = rx * constants::cubic_arc_factor<T>(),
              sy = ry * constants::cubic_arc_factor<T>(),
              x3 = x + sx, x4 = x - sx, y3 = y + sy, y4 = y - sy;
            
            sink(cubic_to, point_type{x1, y3}, point_type{x3, y1}, point_type{x, y1});
            sink(cubic_to, point_type{x4, y1}, point_type{x2, y3}, point_type{x2, y});
            sink(cubic_to, point_type{x2, y4}, point_type{x4, y2}, point_type{x, y2});
            sink(cubic_to, point_type{x3, y2}, point_type{x1, y4}, point_type{x1, y});
#   endif
            sink(end_poly);
        }
    };
}

#endif