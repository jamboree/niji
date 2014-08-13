/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_ADAPTED_DETAIL_RING_ITERATE_HPP_INCLUDED
#define BOOST_NIJI_ADAPTED_DETAIL_RING_ITERATE_HPP_INCLUDED

#include <boost/mpl/if.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/niji/support/command.hpp>

namespace boost { namespace niji { namespace detail
{
    template<geometry::order_selector order, class Ring, class Sink>
    bool ring_iterate(Ring const& ring, Sink& sink)
    {
        using namespace command;
        using range_t = typename
            mpl::if_c
            <
                geometry::point_order<Ring>::value == order
              , Ring const&, reversed_range<Ring const>
            >::type;

        range_t range(ring);
        auto it(boost::begin(range)), end(boost::end(range));

        if (it == end)
            return false;

        sink(move_to, *it);
        while (++it != end)
            sink(line_to, *it);
        sink(end_poly);
        return true;
    }
}}}

#endif
