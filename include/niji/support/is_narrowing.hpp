/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2019-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_IS_NARROWING_HPP_INCLUDED
#define NIJI_SUPPORT_IS_NARROWING_HPP_INCLUDED

#include <type_traits>

namespace niji::detail
{
    template<class From, class To> requires requires(From val) { To{val}; }
    std::false_type is_narrowing_aux(int);

    template<class From, class To>
    std::true_type is_narrowing_aux(...);
}

namespace niji
{
    template<class From, class To>
    struct is_narrowing : std::bool_constant<
        std::is_scalar_v<From>&&
        std::is_scalar_v<To>&&
        decltype(detail::is_narrowing_aux<From, To>(0))::value
    > {};

    template<class T>
    struct is_narrowing<T, T> : std::false_type {};

    template<class From, class To>
    constexpr bool is_narrowing_v = is_narrowing<From, To>::value;
}

#endif