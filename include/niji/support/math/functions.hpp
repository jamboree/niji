/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_MATH_FUNCTIONS_HPP_INCLUDED
#define NIJI_SUPPORT_MATH_FUNCTIONS_HPP_INCLUDED

#include <cmath>
#include <type_traits>
#include <boost/assert.hpp>

namespace niji
{
    template<class T>
    inline T square_sum(T a, T b)
    {
        return a * a + b * b;
    }

    template<class T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
    inline bool is_nearly_zero(T val)
    {
        static constexpr T nearly_zero(T(1) / (1 << 12));
        return val <= nearly_zero;
    }
    
    template<class T>
    int valid_unit_divide(T numer, T denom, T& ratio)
    {
        using std::isnan;

        if (numer < 0)
        {
            numer = -numer;
            denom = -denom;
        }
    
        if (denom == 0 || numer == 0 || numer >= denom)
            return 0;
    
        T r = numer / denom;
        if (isnan(r))
            return 0;
            
        BOOST_ASSERT(r >= 0 && r < 1);
        if (r == 0) // catch underflow if numer <<<< denom
            return 0;
        ratio = r;
        return 1;
    }
    
    template<class T>
    inline T interpolate(T a, T b, T t)
    {
        return a + (b - a) * t;
    }
}

#endif
