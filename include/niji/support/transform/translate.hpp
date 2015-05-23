/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_TRANSFORM_TRANSLATE_HPP_INCLUDED
#define NIJI_SUPPORT_TRANSFORM_TRANSLATE_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <niji/support/point.hpp>

namespace niji { namespace transforms
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
            using boost::geometry::get;
            return {get<0>(pt) + tx, get<1>(pt) + ty};
        }
    };
}}

#endif
