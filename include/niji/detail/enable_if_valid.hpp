/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_DETAIL_ENABLE_IF_VALID_HPP_INCLUDED
#define NIJI_DETAIL_ENABLE_IF_VALID_HPP_INCLUDED

namespace niji { namespace detail
{
    template<class Expr, class T = void>
    struct enable_if_valid
    {
        using type = T;
    };

    template<class T, class U = void>
    using enable_if_valid_t = typename enable_if_valid<T, U>::type;
}}

#endif