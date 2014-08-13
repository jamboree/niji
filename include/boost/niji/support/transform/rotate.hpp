/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_TRANSFORM_ROTATE_HPP_INCLUDED
#define BOOST_NIJI_TRANSFORM_ROTATE_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <boost/niji/support/point.hpp>

namespace boost { namespace niji { namespace transforms
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

        template<class Point>
        point<T> operator()(Point const& pt) const
        {
            using geometry::get;
            auto x = get<0>(pt), y = get<1>(pt);
            return {x * ca - y * sa, x * sa + y * ca};
        }
    };
}}}

#endif
