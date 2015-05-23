/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_JUST_HPP_INCLUDED
#define NIJI_SUPPORT_JUST_HPP_INCLUDED

namespace niji
{
    template<class T>
    struct just
    {
        using type = T;
    };
    
    template<class T>
    using just_t = typename just<T>::type;
}

#endif