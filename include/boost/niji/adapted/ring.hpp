/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_ADAPTED_RING_HPP_INCLUDED
#define BOOST_NIJI_ADAPTED_RING_HPP_INCLUDED

#include <boost/range/adaptor/reversed.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/closure.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/adapted/detail/ring_iterate.hpp>

namespace boost { namespace niji
{
    template<geometry::order_selector order>
    struct geometry_iterate<geometry::ring_tag, order>
    {
        template<class Ring, class Sink>
        static void apply(Ring const& ring, Sink& sink)
        {
            (void)detail::ring_iterate<order>(ring, sink);
        }
    };
}}

#endif
