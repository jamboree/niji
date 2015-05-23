/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_SKIA_HPP_INCLUDED
#define NIJI_SINK_SKIA_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <niji/support/command.hpp>
#include <SkPath.h>

namespace niji
{
    struct skia_sink
    {
        ::SkPath& path;
        
        explicit skia_sink(::SkPath& path)
          : path(path)
        {}
        
        template<class Point>
        void operator()(move_to_t, Point const& pt) const
        {
            using boost::geometry::get;
            path.moveTo(get<0>(pt), get<1>(pt));
        }
        
        template<class Point>
        void operator()(line_to_t, Point const& pt) const
        {
            using boost::geometry::get;
            path.lineTo(get<0>(pt), get<1>(pt));
        }
        
        template<class Point>
        void operator()(quad_to_t, Point const& pt1, Point const& pt2) const
        {
            using boost::geometry::get;
            path.quadTo(get<0>(pt1), get<1>(pt1), get<0>(pt2), get<1>(pt2));
        }
        
        template<class Point>
        void operator()(cubic_to_t, Point const& pt1, Point const& pt2, Point const& pt3) const
        {
            using boost::geometry::get;
            path.cubicTo(get<0>(pt1), get<1>(pt1), get<0>(pt2), get<1>(pt2), get<0>(pt3), get<1>(pt3));
        }
        
        void operator()(end_line_t) const {}
        
        void operator()(end_poly_t) const
        {
            path.close();
        }
    };
}

#endif
