/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_TRANSFORM_SCALE_HPP_INCLUDED
#define BOOST_NIJI_TRANSFORM_SCALE_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <boost/niji/support/point.hpp>

namespace boost { namespace niji { namespace transforms
{
    template<class T>
    struct scale
    {
        T sx, sy;
        
        explicit scale(T scale)
          : sx(scale), sy(scale)
        {}
        
        scale(T sx, T sy)
          : sx(sx), sy(sy)
        {}
        
        void reset(T scale)
        {
            sx = sy = scale;
        }
        
        void reset(T sx2, T sy2)
        {
            sx = sx2;
            sy = sy2;
        }
        
        template<class Point>
        point<T> operator()(Point const& pt) const
        {
            using geometry::get;
            return {get<0>(pt) * sx, get<1>(pt) * sy};
        }
    };
}}}

#endif
