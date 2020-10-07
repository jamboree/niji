/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_IDENTITY_HPP_INCLUDED
#define NIJI_VIEW_IDENTITY_HPP_INCLUDED

namespace niji
{
    struct identity_view {};

    template<class Path>
    constexpr Path&& operator|(Path&& path, identity_view)
    {
        return std::forward<Path>(path);
    }
}

namespace niji::views
{
    constexpr identity_view identity{};
}

#endif