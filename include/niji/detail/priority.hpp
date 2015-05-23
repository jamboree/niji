/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_DETAIL_PRIORITY_HPP_INCLUDED
#define NIJI_DETAIL_PRIORITY_HPP_INCLUDED

namespace niji { namespace detail
{
    template<unsigned N>
    struct priority : priority<N - 1> {};

    template<>
    struct priority<0> {};
}}

#endif