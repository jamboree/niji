/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_TRANSFORM_SKEW_HPP_INCLUDED
#define NIJI_SUPPORT_TRANSFORM_SKEW_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <niji/support/point.hpp>

namespace niji { namespace transforms
{
    template<class T>
    struct skew
    {
        T shx, shy;

        skew(T shx, T shy)
          : shx(shx), shy(shy)
        {}

        void reset(T shx2, T shy2)
        {
            shx = shx2;
            shy = shy2;
        }
        
        template<class Point>
        point<T> operator()(Point const& pt) const
        {
            using boost::geometry::get;
            auto x = get<0>(pt), y = get<1>(pt);
            return {x + shx * y, y + shy * x};
        }
    };
}}

#endif
