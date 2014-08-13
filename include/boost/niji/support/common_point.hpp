/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_COMMON_POINT_HPP_INCLUDED
#define BOOST_NIJI_COMMON_POINT_HPP_INCLUDED

#include <type_traits>
#include <boost/niji/support/point.hpp>

namespace boost { namespace niji { namespace detail
{
    template<class T, class... Ts>
    struct is_all_same
    {
        template<class U>
        using src_t = T;
        
        static constexpr bool value =
            std::is_same<is_all_same, is_all_same<T, src_t<Ts>...>>::value;
    };
    
    template<bool, class... Ts>
    struct common_point_impl;
    
    template<class T, class... Ts>
    struct common_point_impl<true, T, Ts...>
    {
        using type = T;
    };
    
    template<class... Ts>
    struct common_point_impl<false, Ts...>
    {
        using type =
            point<std::common_type_t<
                typename geometry::coordinate_type<Ts>::type...>>;
    };
}}}

namespace boost { namespace niji
{
    template<class... Ts>
    struct common_point
      : detail::common_point_impl<detail::is_all_same<Ts...>::value, Ts...>
    {};

    template<class... Ts>
    using common_point_t = typename common_point<Ts...>::type;
}}

#endif
