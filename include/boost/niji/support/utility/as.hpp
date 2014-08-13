/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_AS_HPP_INCLUDED
#define BOOST_NIJI_AS_HPP_INCLUDED

namespace boost { namespace niji
{
    template<class T>
    struct as
    {
        using type = T;
    };
    
    template<class T>
    using as_t = typename as<T>::type;
}}

#endif
