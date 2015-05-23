/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_TRANSFORM_ROTATE_HPP_INCLUDED
#define NIJI_SUPPORT_TRANSFORM_ROTATE_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <niji/support/point.hpp>

namespace niji { namespace transforms
{
    template<class T>
    struct rotate
    {
        T sa, ca;

        explicit rotate(T radian)
        {
            reset(radian);
        }

        rotate(T sa, T ca)
          : sa(sa), ca(ca)
        {}

        void reset(T radian)
        {
            using std::sin;
            using std::cos;
            
            sa = sin(radian);
            ca = cos(radian);
        }
        
        void reset(T sa2, T ca2)
        {
            sa = sa2;
            ca = ca2;
        }

        template<class Point>
        point<T> operator()(Point const& pt) const
        {
            using boost::geometry::get;
            auto x = get<0>(pt), y = get<1>(pt);
            return {x * ca - y * sa, x * sa + y * ca};
        }
    };
}}

#endif
