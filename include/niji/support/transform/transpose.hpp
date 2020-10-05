/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.xt or copy at http://www.boost.org/LICENSE_1_0.xt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_TRANSFORM_TRANSPOSE_HPP_INCLUDED
#define NIJI_SUPPORT_TRANSFORM_TRANSPOSE_HPP_INCLUDED

#include <niji/support/point.hpp>

namespace niji::transforms
{
    struct transpose
    {
        template<class Point>
        auto operator()(Point const& pt) const
        {
            return point(get_y(pt), get_x(pt));
        }
    };
}

#endif