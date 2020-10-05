/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.xt or copy at http://www.boost.org/LICENSE_1_0.xt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_TRANSFORM_TRANSLATE_HPP_INCLUDED
#define NIJI_SUPPORT_TRANSFORM_TRANSLATE_HPP_INCLUDED

#include <niji/support/point.hpp>

namespace niji::transforms
{
    template<class T>
    struct translate
    {
        T x, y;

        translate(T x, T y)
          : x(x), y(y)
        {}

        explicit translate(point<T> const& v)
            : x(v.x), y(v.y)
        {}

        void reset(T x2, T y2)
        {
            x = x2;
            y = y2;
        }

        template<class Point>
        point<T> operator()(Point const& pt) const
        {
            return {get_x(pt) + x, get_y(pt) + y};
        }
    };
}

#endif