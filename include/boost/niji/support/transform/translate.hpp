/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_TRANSFORM_TRANSLATE_HPP_INCLUDED
#define BOOST_NIJI_TRANSFORM_TRANSLATE_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <boost/niji/support/point.hpp>

namespace boost { namespace niji { namespace transforms
{
    template<class T>
    struct translate
    {
        T dx, dy;

        translate(T dx, T dy)
          : dx(dx), dy(dy)
        {}

        void reset(T dx2, T dy2)
        {
            dx = dx2;
            dy = dy2;
        }

        template<class Point>
        point<T> operator()(Point const& pt) const
        {
            using geometry::get;
            return {get<0>(pt) + dx, get<1>(pt) + dy};
        }
    };
}}}

#endif
