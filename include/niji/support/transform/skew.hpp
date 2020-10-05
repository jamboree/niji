/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_TRANSFORM_SKEW_HPP_INCLUDED
#define NIJI_SUPPORT_TRANSFORM_SKEW_HPP_INCLUDED

#include <niji/support/point.hpp>

namespace niji::transforms
{
    template<class T>
    struct skew
    {
        T x, y;

        skew(T x, T y)
          : x(x), y(y)
        {}

        void reset(T x2, T y2)
        {
            x = x2;
            y = y2;
        }
        
        template<class Point>
        point<T> operator()(Point const& pt) const
        {
            auto x2 = get_x(pt), y2 = get_y(pt);
            return {x2 + x * y2, y2 + y * x2};
        }
    };
}

#endif