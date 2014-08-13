/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_ADAPTED_LINESTRING_HPP_INCLUDED
#define BOOST_NIJI_ADAPTED_LINESTRING_HPP_INCLUDED

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/traits.hpp>

namespace boost { namespace niji
{
    template<>
    struct geometry_iterate<geometry::linestring_tag>
    {
        template<class Linestring, class Sink>
        static void apply(Linestring const& linestring, Sink& sink)
        {
            using namespace command;

            auto it(boost::begin(linestring)), end(boost::end(linestring));

            if (it == end)
                return;

            sink(move_to, *it);
            while (++it != end)
                sink(line_to, *it);
            sink(end_line);
        }
    };
    
    template<geometry::order_selector order>
    struct geometry_iterate<geometry::multi_linestring_tag, order>
      : multi_geometry_iterate<geometry::linestring_tag, order>
    {};
}}

#endif
