/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_HOLDER_HPP_INCLUDED
#define BOOST_NIJI_HOLDER_HPP_INCLUDED

#include <type_traits>
#include <boost/mpl/if.hpp>

namespace boost { namespace niji
{
    template<class T>
    struct holder
    {
        using type = T;
    };
    
    template<class T>
    struct holder<T&>
      : mpl::if_<std::is_empty<T>, std::remove_cv_t<T>, T&>
    {};
    
    template<class T>
    using holder_t = typename holder<T>::type;
}}

#endif
