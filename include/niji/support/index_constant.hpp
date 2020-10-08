/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_INDEX_HPP_INCLUDED
#define NIJI_SUPPORT_INDEX_HPP_INCLUDED

#include <type_traits>

namespace niji
{
    template<int I>
    using index_constant = std::integral_constant<int, I>;

    template<int I>
    constexpr index_constant<I> index{};
}

#endif