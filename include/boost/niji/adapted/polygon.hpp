/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_ADAPTED_POLYGON_HPP_INCLUDED
#define BOOST_NIJI_ADAPTED_POLYGON_HPP_INCLUDED

#include <boost/range/adaptor/reversed.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/closure.hpp>
#include <boost/geometry/core/ring_type.hpp>
#include <boost/geometry/core/interior_type.hpp>
#include <boost/geometry/core/exterior_ring.hpp>
#include <boost/geometry/core/interior_rings.hpp>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/adapted/detail/ring_iterate.hpp>

namespace boost { namespace niji
{
    template<geometry::order_selector order>
    struct geometry_iterate<geometry::polygon_tag, order>
    {
        template<class Polygon, class Sink>
        static void apply(Polygon const& polygon, Sink& sink)
        {
            if (detail::ring_iterate<order>(geometry::exterior_ring(polygon), sink))
            {
                for (auto const& ring : geometry::interior_rings(polygon))
                    detail::ring_iterate<order>(ring, sink);
            }
        }
    };
    
    template<geometry::order_selector order>
    struct geometry_iterate<geometry::multi_polygon_tag, order>
      : multi_geometry_iterate<geometry::polygon_tag, order>
    {};
}}

#endif
