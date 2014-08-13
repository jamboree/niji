/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_MATH_FUNCTIONS_HPP_INCLUDED
#define BOOST_NIJI_MATH_FUNCTIONS_HPP_INCLUDED

#include <boost/type_traits/is_floating_point.hpp>
#include <boost/niji/support/utility/enable_if.hpp>

namespace boost { namespace niji
{
    template<class T>
    inline T square_sum(T a, T b)
    {
        return a * a + b * b;
    }

    template<class T, enable_if_t<is_floating_point<T>> = true>
    inline bool is_nearly_zero(T val)
    {
        static constexpr T nearly_zero(T(1) / (1 << 12));
        return val <= nearly_zero;
    }
    
    template<class T>
    int valid_unit_divide(T numer, T denom, T& ratio)
    {
        if (numer < 0)
        {
            numer = -numer;
            denom = -denom;
        }
    
        if (denom == 0 || numer == 0 || numer >= denom)
            return 0;
    
        T r = numer / denom;
        if (math::isnan(r))
            return 0;
            
        BOOST_ASSERT(r >= 0 && r < 1);
        if (r == 0) // catch underflow if numer <<<< denom
            return 0;
        ratio = r;
        return 1;
    }
}}

#endif
