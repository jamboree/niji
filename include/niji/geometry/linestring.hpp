/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GEOMETRY_LINESTRING_HPP_INCLUDED
#define NIJI_GEOMETRY_LINESTRING_HPP_INCLUDED

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/geometry/core/access.hpp>
#include <niji/support/command.hpp>
#include <niji/geometry/render.hpp>
#include <niji/geometry/detail/multi_geometry_render.hpp>

namespace niji
{
    template<>
    struct geometry_render<boost::geometry::linestring_tag, boost::geometry::counterclockwise>
    {
        template<class Linestring, class Sink>
        static void apply(Linestring const& linestring, Sink& sink)
        {
            using namespace command;
            using std::begin;
            using std::end;

            auto it(begin(linestring)), e(end(linestring));

            if (it == e)
                return;

            sink(move_to, *it);
            while (++it != e)
                sink(line_to, *it);
            sink(end_line);
        }
    };
    
    template<>
    struct geometry_render<boost::geometry::linestring_tag, boost::geometry::clockwise>
      : geometry_render<boost::geometry::linestring_tag, boost::geometry::counterclockwise>
    {};

    template<boost::geometry::order_selector order>
    struct geometry_render<boost::geometry::multi_linestring_tag, order>
      : detail::multi_geometry_render<boost::geometry::linestring_tag, order>
    {};
}

#endif