/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_D2D_HPP_INCLUDED
#define NIJI_SINK_D2D_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <niji/support/command.hpp>
#include <d2d1.h>

namespace niji
{
    struct d2d_sink
    {
        ::ID2D1GeometrySink* geometry_sink;
        ::D2D1_FIGURE_BEGIN figure_begin;
        
        d2d_sink(::ID2D1GeometrySink* d2d_sink, ::D2D1_FIGURE_BEGIN figure_begin)
          : geometry_sink(d2d_sink), figure_begin(figure_begin)
        {}

        template<class Point>
        static ::D2D1_POINT_2F cvt_point(Point const& pt)
        {
            using boost::geometry::get;
            ::D2D1_POINT_2F ret = {get<0>(pt), get<1>(pt)};
            return ret;
        }
        
        template<class Point>
        void operator()(move_to_t, Point const& pt) const
        {
            geometry_sink->BeginFigure(cvt_point(pt), figure_begin);
        }
        
        template<class Point>
        void operator()(line_to_t, Point const& pt) const
        {
            geometry_sink->AddLine(cvt_point(pt));
        }
        
        template<class Point>
        void operator()(quad_to_t, Point const& pt1, Point const& pt2) const
        {
            geometry_sink->AddQuadraticBezier(
            {
                cvt_point(pt1), cvt_point(pt2)
            });
        }
        
        template<class Point>
        void operator()(cubic_to_t, Point const& pt1, Point const& pt2, Point const& pt3) const
        {
            using boost::geometry::get;
            geometry_sink->AddBezier(
            {
                cvt_point(pt1), cvt_point(pt2), cvt_point(pt3)
            });
        }
        
        void operator()(end_line_t) const
        {
            geometry_sink->EndFigure(::D2D1_FIGURE_END_OPEN);
        }
        
        void operator()(end_poly_t) const
        {
            geometry_sink->EndFigure(::D2D1_FIGURE_END_CLOSED);
        }
    };
}

#endif