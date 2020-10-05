/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_TRANSFORM_ROTATE_HPP_INCLUDED
#define NIJI_SUPPORT_TRANSFORM_ROTATE_HPP_INCLUDED

#include <niji/support/point.hpp>

namespace niji::transforms
{
    template<class T>
    struct rotate
    {
        T sin, cos;

        explicit rotate(T radian)
        {
            reset(radian);
        }

        rotate(T sin, T cos)
          : sin(sin), cos(cos)
        {}

        void reset(T radian)
        {
            [](T radian, rotate& self)
            {
                using std::sin;
                using std::cos;

                self.sin = sin(radian);
                self.cos = cos(radian);
            }(radian, *this);
        }
        
        void reset(T sa2, T ca2)
        {
            sin = sa2;
            cos = ca2;
        }

        template<class Point>
        point<T> operator()(Point const& pt) const
        {
            auto x = get_x(pt), y = get_y(pt);
            return {x * cos - y * sin, x * sin + y * cos};
        }
    };
}

#endif