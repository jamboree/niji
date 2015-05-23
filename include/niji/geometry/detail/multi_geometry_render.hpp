/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GEOMETRY_DETAIL_MULTI_GEOMETRY_ITERATE_HPP_INCLUDED
#define NIJI_GEOMETRY_DETAIL_MULTI_GEOMETRY_ITERATE_HPP_INCLUDED

#include <boost/geometry/core/point_order.hpp>

namespace niji { namespace detail
{
    template<class Tag, boost::geometry::order_selector order>
    struct multi_geometry_render
    {
        template<class MultiGeometry, class Sink>
        static void apply(MultiGeometry const& multi_geometry, Sink& sink)
        {
            for (auto const& geometry : multi_geometry)
                geometry_render<Tag, order>::apply(geometry, sink);
        }
    };
}}

#endif
