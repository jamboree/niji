/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_TRANSFORM_SCALE_HPP_INCLUDED
#define NIJI_SUPPORT_TRANSFORM_SCALE_HPP_INCLUDED

#include <niji/support/point.hpp>

namespace niji::transforms
{
    template<class T>
    struct scale
    {
        T x, y;
        
        explicit scale(T scale)
          : x(scale), y(scale)
        {}
        
        scale(T x, T y)
          : x(x), y(y)
        {}
        
        void reset(T scale)
        {
            x = y = scale;
        }
        
        void reset(T x2, T y2)
        {
            x = x2;
            y = y2;
        }
        
        template<class Point>
        point<T> operator()(Point const& pt) const
        {
            return {get_x(pt) * x, get_y(pt) * y};
        }
    };
}

#endif