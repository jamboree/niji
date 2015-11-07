/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_CAIRO_HPP_INCLUDED
#define NIJI_SINK_CAIRO_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <niji/support/command.hpp>
#include <niji/support/point.hpp>
#include <cairo.h>

namespace niji
{
    struct cairo_sink
    {
        ::cairo_t* context;
        
        explicit cairo_sink(::cairo_t* context)
          : context(context)
        {}
        
        template<class Point>
        void operator()(move_to_t, Point const& pt) const
        {
            using boost::geometry::get;
            ::cairo_move_to(context, get<0>(pt), get<1>(pt));
        }
        
        template<class Point>
        void operator()(line_to_t, Point const& pt) const
        {
            using boost::geometry::get;
            ::cairo_line_to(context, get<0>(pt), get<1>(pt));
        }

        void operator()(quad_to_t, dpoint pt1, dpoint const& pt2) const
        {
            dpoint pt0;
            ::cairo_get_current_point(context, &pt0.x, &pt0.y);
            pt1 *= 2.0;
            pt0 = (pt0 + pt1) / 3.0;
            pt1 = (pt2 + pt1) / 3.0;
            ::cairo_curve_to(context, pt0.x, pt0.y, pt1.x, pt1.y, pt2.x, pt2.y);
        }
        
        template<class Point>
        void operator()(cubic_to_t, Point const& pt1, Point const& pt2, Point const& pt3) const
        {
            using boost::geometry::get;
            ::cairo_curve_to(context, get<0>(pt1), get<1>(pt1), get<0>(pt2), get<1>(pt2), get<0>(pt3), get<1>(pt3));
        }
        
        void operator()(end_line_t) const {}

        void operator()(end_poly_t) const
        {
            ::cairo_close_path(context);
        }
    };
}

#endif