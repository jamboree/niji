/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GEOMETRY_RING_HPP_INCLUDED
#define NIJI_GEOMETRY_RING_HPP_INCLUDED

#include <boost/range/adaptor/reversed.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/closure.hpp>
#include <niji/geometry/render.hpp>
#include <niji/geometry/detail/ring_render.hpp>

namespace niji
{
    template<boost::geometry::order_selector order>
    struct geometry_render<boost::geometry::ring_tag, order>
    {
        template<class Ring, class Sink>
        static void apply(Ring const& ring, Sink& sink)
        {
            (void)detail::ring_render<order>(ring, sink);
        }
    };
}

#endif