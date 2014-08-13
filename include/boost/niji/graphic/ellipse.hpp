/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_GRAPHIC_ELLIPSE_HPP_INCLUDED
#define BOOST_NIJI_GRAPHIC_ELLIPSE_HPP_INCLUDED

#include <boost/niji/support/command.hpp>
#include <boost/niji/support/math/constants.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/point.hpp>

namespace boost { namespace niji
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
        void iterate(Sink& sink) const
        {
            iterate_impl(sink, ry);
        }
        
        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            iterate_impl(sink, -ry);
        }
        
    private:
         
        template<class Sink>
        void iterate_impl(Sink& sink, T ry) const
        {
            using namespace command;

            T x = origin.x, y = origin.y,
              x1 = x + rx, x2 = x - rx,
              y1 = y + ry, y2 = y - ry,
              sx = rx * constants::tan_pi_over_8<T>(),
              sy = ry * constants::tan_pi_over_8<T>(),
              mx = rx * constants::root2_over_2<T>(),
              my = ry * constants::root2_over_2<T>();
                   
            sink(move_to, point_type{x1, y});
            sink(quad_to, point_type{x1, y + sy}, point_type{x + mx, y + my});
            sink(quad_to, point_type{x + sx, y1}, point_type{x, y1});
            sink(quad_to, point_type{x - sx, y1}, point_type{x - mx, y + my});
            sink(quad_to, point_type{x2, y + sy}, point_type{x2, y});
            sink(quad_to, point_type{x2, y - sy}, point_type{x - mx, y - my});
            sink(quad_to, point_type{x - sx, y2}, point_type{x, y2});
            sink(quad_to, point_type{x + sx, y2}, point_type{x + mx, y - my});
            sink(quad_to, point_type{x1, y - sy}, point_type{x1, y});
            sink(end_poly);
        }
    };
}}

#endif
