/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_CONVERT_GEOMETRY_HPP_INCLUDED
#define NIJI_SUPPORT_CONVERT_GEOMETRY_HPP_INCLUDED

#include <type_traits>
#include <boost/geometry/algorithms/convert.hpp>

namespace niji
{
    template<class T>
    inline T const& convert_geometry(T const& src)
    {
        return src;
    }
    
    template<class T, class U>
    inline typename std::enable_if<!std::is_convertible<U, T>::value, T>::type
    convert_geometry(U const& src)
    {
        T dst;
        boost::geometry::convert(src, dst);
        return dst;
    }
}

#endif
