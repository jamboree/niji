/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2018 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_NUMERIC_HPP_INCLUDED
#define NIJI_SUPPORT_NUMERIC_HPP_INCLUDED

#include <cmath>
#include <cassert>
#include <type_traits>

namespace niji
{
    template<class T>
    concept Float = std::is_floating_point_v<T>;
}

namespace niji::numeric
{
    struct one
    {
        template<class T>
        T&& operator*(T&& t) const
        {
            return std::forward<T>(t);
        }
    };

    struct zero
    {
        template<class T>
        T&& operator+(T&& t) const
        {
            return std::forward<T>(t);
        }
    };

    template<class T>
    inline T square_sum(T a, T b)
    {
        return a * a + b * b;
    }

    template<Float T>
    inline bool is_nearly_zero(T val)
    {
        static constexpr T nearly_zero = T(1) / (1 << 12);
        return val <= nearly_zero;
    }

    template<class T>
    inline int get_sign(T x)
    {
        return x < 0 ? -1 : (x > 0);
    }

    template<class T>
    inline bool between(T a, T b, T c)
    {
        assert(((a <= b && b <= c) || (a >= b && b >= c)) == ((a - b) * (c - b) <= 0)
            || (is_nearly_zero(a) && is_nearly_zero(b) && is_nearly_zero(c)));
        return (a - b) * (c - b) <= 0;
    }

    template<class T>
    inline bool is_not_monotonic(T a, T b, T c)
    {
        T ab = a - b;
        T bc = b - c;
        if (ab < 0)
            bc = -bc;
        return ab == 0 || bc < 0;
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
            
        assert(r >= 0 && r < 1);
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