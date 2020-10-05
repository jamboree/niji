/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2019 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_VECTOR_HPP_INCLUDED
#define NIJI_SUPPORT_VECTOR_HPP_INCLUDED

#include <type_traits>
#include <niji/support/point.hpp>
#include <niji/support/numbers.hpp>
#include <niji/support/numeric.hpp>

namespace niji
{
    template<class T>
    using vector = point<T>;
    using ivector = vector<int>;
    using fvector = vector<float>;
    using dvector = vector<double>;
}

namespace niji::vectors
{
    template<class T>
    inline T norm_square(vector<T> const& v)
    {
        return numeric::square_sum(v.x, v.y);
    }

    template<class T>
    inline T norm(vector<T> const& v)
    {
        using std::sqrt;
        return sqrt(norm_square(v));
    }

    template<class T>
    inline vector<T> normal_cw(vector<T> const& v)
    {
        return vector<T>(v.y, -v.x);
    }

    template<class T>
    inline vector<T> normal_ccw(vector<T> const& v)
    {
        return vector<T>(-v.y, v.x);
    }
    
    template<class T>
    inline vector<T> unit(vector<T> const& v)
    {
        return v / norm(v);
    }
    
    template<class T, class U>
    inline typename std::common_type<T, U>::type
    dot(vector<T> const& a, vector<U> const& b)
    {
        return a.x * b.x + a.y * b.y;
    }
    
    template<class T, class U>
    inline typename std::common_type<T, U>::type
    cross(vector<T> const& a, vector<U> const& b)
    {
        return a.x * b.y - a.y * b.x;
    }
    
    template<class T, class U>
    inline bool is_ccw(vector<T> const& from, vector<U> const& to)
    {
        return from.x * to.y > from.y * to.x;
    }
}

namespace niji::vectors::traits
{
    //
    // Default Implemetation
    //
    template<Float T>
    struct is_degenerated
    {
        static bool apply(vector<T> const& v)
        {
            static constexpr T nearly_zero(T(1) / (1 << 12));
            return norm(v) <= nearly_zero;
        }
    };
    
    template<Float T>
    struct too_curvy
    {
        static bool apply(vector<T> const& v1, vector<T> const& v2)
        {
            using std::sqrt;
            static T const limit((numbers::root_two<T> * 5 + 1) / 10);
            return dot(v1, v2) <= limit * sqrt(norm_square(v1) * norm_square(v2));
        }
    };

    template<Float T>
    struct too_pinchy
    {
        static bool apply(vector<T> const& v1, vector<T> const& v2)
        {
            using std::sqrt;
            static T const limit(std::numeric_limits<T>::epsilon() * 256 - 1);
            return dot(v1, v2) <= limit * sqrt(norm_square(v1) * norm_square(v2));
        }
    };
}

namespace niji::vectors
{    
    template<class T>
    inline bool is_degenerated(vector<T> const& v)
    {
        return traits::is_degenerated<T>::apply(v);
    }
    
    template<class T>
    inline bool too_curvy(vector<T> const& v1, vector<T> const& v2)
    {
        return traits::too_curvy<T>::apply(v1, v2);
    }
    
    template<class T>
    inline bool too_pinchy(vector<T> const& v1, vector<T> const& v2)
    {
        return traits::too_pinchy<T>::apply(v1, v2);
    }
}

#endif