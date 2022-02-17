/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_D2D_HPP_INCLUDED
#define NIJI_SINK_D2D_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
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
        void move_to(Point const& pt)
        {
            geometry_sink->BeginFigure(cvt_point(pt), figure_begin);
        }
        
        template<class Point>
        void line_to(Point const& pt)
        {
            geometry_sink->AddLine(cvt_point(pt));
        }
        
        template<class Point>
        void quad_to(Point const& pt1, Point const& pt2)
        {
            geometry_sink->AddQuadraticBezier(
            {
                cvt_point(pt1), cvt_point(pt2)
            });
        }
        
        template<class Point>
        void cubic_to(Point const& pt1, Point const& pt2, Point const& pt3)
        {
            using boost::geometry::get;
            geometry_sink->AddBezier(
            {
                cvt_point(pt1), cvt_point(pt2), cvt_point(pt3)
            });
        }
        
        void end_open()
        {
            geometry_sink->EndFigure(::D2D1_FIGURE_END_OPEN);
        }
        
        void end_closed()
        {
            geometry_sink->EndFigure(::D2D1_FIGURE_END_CLOSED);
        }
    };
}

#endif