/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2016 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_MATH_INVARIANT_HPP_INCLUDED
#define NIJI_SUPPORT_MATH_INVARIANT_HPP_INCLUDED

namespace niji
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
}

#endif