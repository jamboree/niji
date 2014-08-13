/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_SINK_D2D_HPP_INCLUDED
#define BOOST_NIJI_SINK_D2D_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <boost/niji/support/command.hpp>
#include <d2d1.h>

namespace boost { namespace niji
{
    struct d2d_sink
    {
        ::ID2D1GeometrySink* geometry_sink;
        ::D2D1_FIGURE_BEGIN figure_begin;
        
        d2d_sink(::ID2D1GeometrySink* d2d_sink, ::D2D1_FIGURE_BEGIN figure_begin)
          : geometry_sink(d2d_sink), figure_begin(figure_begin)
        {}
        
        template<class Point>
        void operator()(move_to_t, Point const& pt) const
        {
            using geometry::get;
            geometry_sink->BeginFigure({get<0>(pt), get<1>(pt)}, figure_begin);
        }
        
        template<class Point>
        void operator()(line_to_t, Point const& pt) const
        {
            using geometry::get;
            geometry_sink->AddLine({get<0>(pt), get<1>(pt)});
        }
        
        template<class Point>
        void operator()(quad_to_t, Point const& pt1, Point const& pt2) const
        {
            using geometry::get;
            geometry_sink->AddQuadraticBezier(
            {
                {get<0>(pt1), get<1>(pt1)}
              , {get<0>(pt2), get<1>(pt2)}
            });
        }
        
        template<class Point>
        void operator()(cubic_to_t, Point const& pt1, Point const& pt2, Point const& pt3) const
        {
            using geometry::get;
            geometry_sink->AddBezier(
            {
                {get<0>(pt1), get<1>(pt1)}
              , {get<0>(pt2), get<1>(pt2)}
              , {get<0>(pt3), get<1>(pt3)}
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
}}

#endif
