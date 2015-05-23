/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_DETAIL_COORD_FN_HPP_INCLUDED
#define NIJI_DETAIL_COORD_FN_HPP_INCLUDED

#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/access.hpp>

namespace niji { namespace detail
{
    template<unsigned N>
    struct coord_fn
    {
        template<class Point>
        decltype(auto) operator()(Point const& pt) const
        {
            return boost::geometry::get<N>(pt);
        }
    };
}}

#endif