/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_ENABLE_IF_HPP_INCLUDED
#define BOOST_NIJI_ENABLE_IF_HPP_INCLUDED

#include <boost/utility/enable_if.hpp>

namespace boost { namespace niji
{
    template<class Expr, class T = void>
    struct enable_if_valid
    {
        using type = T;
    };
    
    template<bool flag, class T = void>
    using enable_if_c_t = typename enable_if_c<flag, T>::type;
    
    template<class T, class U = void>
    using enable_if_t = typename enable_if<T, U>::type;
    
    template<class T, class U = void>
    using enable_if_valid_t = typename enable_if_valid<T, U>::type;
}}

#endif
