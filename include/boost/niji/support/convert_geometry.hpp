/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_CONVERT_GEOMETRY_HPP_INCLUDED
#define BOOST_NIJI_CONVERT_GEOMETRY_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/geometry/algorithms/convert.hpp>

namespace boost { namespace niji
{
    template<class T>
    inline T const& convert_geometry(T const& src)
    {
        return src;
    }
    
    template<class T, class U>
    inline typename disable_if<is_convertible<U, T>, T>::type
    convert_geometry(U const& src)
    {
        T dst;
        geometry::convert(src, dst);
        return dst;
    }
}}

#endif
