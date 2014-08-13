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
        T x_scale, y_scale;
        
        explicit scale(T scale)
          : x_scale(scale), y_scale(scale)
        {}
        
        scale(T x_scale, T y_scale)
          : x_scale(x_scale), y_scale(y_scale)
        {}
        
        void reset(T scale)
        {
            x_scale = y_scale = scale;
        }
        
        void reset(T x_scale2, T y_scale2)
        {
            x_scale = x_scale2;
            y_scale = y_scale2;
        }
        
        template<class Point>
        point<T> operator()(Point const& pt) const
        {
            using geometry::get;
            return {get<0>(pt) * x_scale, get<1>(pt) * y_scale};
        }
    };
}}}

#endif
