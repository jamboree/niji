/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GEOMETRY_POLYGON_HPP_INCLUDED
#define NIJI_GEOMETRY_POLYGON_HPP_INCLUDED

#include <boost/range/adaptor/reversed.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/closure.hpp>
#include <boost/geometry/core/ring_type.hpp>
#include <boost/geometry/core/interior_type.hpp>
#include <boost/geometry/core/exterior_ring.hpp>
#include <boost/geometry/core/interior_rings.hpp>
#include <niji/support/command.hpp>
#include <niji/geometry/render.hpp>
#include <niji/geometry/detail/ring_render.hpp>
#include <niji/geometry/detail/multi_geometry_render.hpp>

namespace niji
{
    template<boost::geometry::order_selector order>
    struct geometry_render<boost::geometry::polygon_tag, order>
    {
        template<class Polygon, class Sink>
        static void apply(Polygon const& polygon, Sink& sink)
        {
            if (detail::ring_render<order>(boost::geometry::exterior_ring(polygon), sink))
            {
                for (auto const& ring : boost::geometry::interior_rings(polygon))
                    detail::ring_render<order>(ring, sink);
            }
        }
    };
    
    template<boost::geometry::order_selector order>
    struct geometry_render<boost::geometry::multi_polygon_tag, order>
      : detail::multi_geometry_render<boost::geometry::polygon_tag, order>
    {};
}

#endif
