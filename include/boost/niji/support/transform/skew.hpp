/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_TRANSFORM_SKEW_HPP_INCLUDED
#define BOOST_NIJI_TRANSFORM_SKEW_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <boost/niji/support/point.hpp>

namespace boost { namespace niji { namespace transforms
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
            using geometry::get;
            auto x = get<0>(pt), y = get<1>(pt);
            return {x + shx * y, y + shy * x};
        }
    };
}}}

#endif
