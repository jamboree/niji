/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GEOMETRY_DETAIL_RING_ITERATE_HPP_INCLUDED
#define NIJI_GEOMETRY_DETAIL_RING_ITERATE_HPP_INCLUDED

#include <type_traits>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <niji/support/command.hpp>

namespace niji { namespace detail
{
    template<boost::geometry::order_selector order, class Ring, class Sink>
    bool ring_render(Ring const& ring, Sink& sink)
    {
        using namespace command;
        using std::begin;
        using std::end;
        using range_t = std::conditional_t
            <
                boost::geometry::point_order<Ring>::value == order
              , Ring const&, boost::reversed_range<Ring const>
            >;

        range_t range(ring);
        auto it(begin(range)), e(end(range));

        if (it == e)
            return false;

        sink(move_to, *it);
        while (++it != e)
            sink(line_to, *it);
        sink(end_poly);
        return true;
    }
}}

#endif
