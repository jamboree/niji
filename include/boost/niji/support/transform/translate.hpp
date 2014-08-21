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
        T tx, ty;

        translate(T tx, T ty)
          : tx(tx), ty(ty)
        {}

        void reset(T tx2, T ty2)
        {
            tx = tx2;
            ty = ty2;
        }

        template<class Point>
        point<T> operator()(Point const& pt) const
        {
            using geometry::get;
            return {get<0>(pt) + tx, get<1>(pt) + ty};
        }
    };
}}}

#endif
