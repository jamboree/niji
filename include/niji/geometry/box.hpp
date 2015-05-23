/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GEOMETRY_BOX_HPP_INCLUDED
#define NIJI_GEOMETRY_BOX_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <boost/geometry/algorithms/make.hpp>
#include <niji/support/command.hpp>
#include <niji/geometry/render.hpp>
#include <niji/support/point.hpp>

namespace niji
{
    template<>
    struct geometry_render<boost::geometry::box_tag, boost::geometry::counterclockwise>
    {
        template<class Box, class Sink>
        static void apply(Box const& box, Sink& sink)
        {
            using namespace command;
            using boost::geometry::get;
            using boost::geometry::make;
            using coord_t = typename boost::geometry::coordinate_type<Box>::type;
            using point_t = typename boost::geometry::point_type<Box>::type;

            coord_t x1 = get<boost::geometry::min_corner, 0>(box);
            coord_t y1 = get<boost::geometry::min_corner, 1>(box);
            coord_t x2 = get<boost::geometry::max_corner, 0>(box);
            coord_t y2 = get<boost::geometry::max_corner, 1>(box);
            sink(move_to, make<point_t>(x1, y1));
            sink(line_to, make<point_t>(x2, y1));
            sink(line_to, make<point_t>(x2, y2));
            sink(line_to, make<point_t>(x1, y2));
            sink(end_poly);
        }
    };
    
    template<>
    struct geometry_render<boost::geometry::box_tag, boost::geometry::clockwise>
    {
        template<class Box, class Sink>
        static void apply(Box const& box, Sink& sink)
        {
            using namespace command;
            using boost::geometry::get;
            using boost::geometry::make;
            using coord_t = typename boost::geometry::coordinate_type<Box>::type;
            using point_t = typename boost::geometry::point_type<Box>::type;

            coord_t x1 = get<boost::geometry::min_corner, 0>(box);
            coord_t y1 = get<boost::geometry::min_corner, 1>(box);
            coord_t x2 = get<boost::geometry::max_corner, 0>(box);
            coord_t y2 = get<boost::geometry::max_corner, 1>(box);
            sink(move_to, make<point_t>(x1, y1));
            sink(line_to, make<point_t>(x1, y2));
            sink(line_to, make<point_t>(x2, y2));
            sink(line_to, make<point_t>(x2, y1));
            sink(end_poly);
        }
    };
}

#endif