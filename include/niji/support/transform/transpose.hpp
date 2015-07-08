/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.xt or copy at http://www.boost.org/LICENSE_1_0.xt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_TRANSFORM_TRANSPOSE_HPP_INCLUDED
#define NIJI_SUPPORT_TRANSFORM_TRANSPOSE_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <boost/geometry/algorithms/make.hpp>
#include <niji/support/point.hpp>

namespace niji { namespace transforms
{
    struct transpose
    {
        template<class Point>
        Point operator()(Point const& pt) const
        {
            using boost::geometry::get;
            return boost::geometry::make(get<1>(pt), get<0>(pt));
        }
    };
}}

#endif