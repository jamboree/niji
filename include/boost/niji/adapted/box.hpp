/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_ADAPTED_BOX_HPP_INCLUDED
#define BOOST_NIJI_ADAPTED_BOX_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <boost/geometry/algorithms/make.hpp>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/point.hpp>

namespace boost { namespace niji
{
    template<>
    struct geometry_iterate<geometry::box_tag, geometry::counterclockwise>
    {
        template<class Box, class Sink>
        static void apply(Box const& box, Sink& sink)
        {
            using namespace command;
            using geometry::get;
            using geometry::make;
            using coord_t = typename geometry::coordinate_type<Box>::type;
            using point_t = typename geometry::point_type<Box>::type;

            coord_t x1 = get<geometry::min_corner, 0>(box);
            coord_t y1 = get<geometry::min_corner, 1>(box);
            coord_t x2 = get<geometry::max_corner, 0>(box);
            coord_t y2 = get<geometry::max_corner, 1>(box);
            sink(move_to, make<point_t>(x1, y1));
            sink(line_to, make<point_t>(x2, y1));
            sink(line_to, make<point_t>(x2, y2));
            sink(line_to, make<point_t>(x1, y2));
            sink(end_poly);
        }
    };
    
    template<>
    struct geometry_iterate<geometry::box_tag, geometry::clockwise>
    {
        template<class Box, class Sink>
        static void apply(Box const& box, Sink& sink)
        {
            using namespace command;
            using geometry::get;
            using geometry::make;
            using coord_t = typename geometry::coordinate_type<Box>::type;
            using point_t = typename geometry::point_type<Box>::type;

            coord_t x1 = get<geometry::min_corner, 0>(box);
            coord_t y1 = get<geometry::min_corner, 1>(box);
            coord_t x2 = get<geometry::max_corner, 0>(box);
            coord_t y2 = get<geometry::max_corner, 1>(box);
            sink(move_to, make<point_t>(x1, y1));
            sink(line_to, make<point_t>(x1, y2));
            sink(line_to, make<point_t>(x2, y2));
            sink(line_to, make<point_t>(x2, y1));
            sink(end_poly);
        }
    };
}}

#endif
