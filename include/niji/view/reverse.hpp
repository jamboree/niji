/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_REVERSE_HPP_INCLUDED
#define NIJI_VIEW_REVERSE_HPP_INCLUDED

#include <niji/support/view.hpp>

namespace niji
{
    struct reverse_view : view<reverse_view>
    {
        template<BiPath P, class Sink>
        static void iterate(P const& path, Sink& sink)
        {
             niji::reverse_iterate(path, sink);
        }
                
        template<Path P, class Sink>
        static void reverse_iterate(P const& path, Sink& sink)
        {
             niji::iterate(path, sink);
        }
    };
}

namespace niji::views
{
    constexpr reverse_view reverse{};
}

#endif