/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2017-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_FORK_HPP_INCLUDED
#define NIJI_VIEW_FORK_HPP_INCLUDED

#include <tuple>
#include <type_traits>
#include <niji/support/view.hpp>

namespace niji
{
    template<class... Views>
    struct fork_view : view<fork_view<Views...>>
    {
        using indices_t = std::make_index_sequence<sizeof...(Views)>;

        std::tuple<Views...> branches;

        fork_view(Views... branches)
          : branches(std::forward<Views>(branches)...)
        {}

        template<Path P, class Sink>
        void iterate(P const& path, Sink& sink) const
        {
            iterate_impl(niji::iterate, path, sink, indices_t{});
        }

        template<Path P, class Sink>
        void reverse_iterate(P const& path, Sink& sink) const requires (BiPath<std::decay_t<decltype(path | std::declval<Views>())>> && ...)
        {
            iterate_impl(niji::reverse_iterate, path, sink, indices_t{});
        }

    private:
        template<class F, class Path, class Sink, std::size_t... I>
        void iterate_impl(F f, Path const& path, Sink& sink, std::index_sequence<I...>) const
        {
            (f(path | std::get<I>(branches), sink), ...);
        }
    };
}

namespace niji::views
{
    template<class... Views>
    inline fork_view<Views...> fork(Views&&... branches)
    {
        return {std::forward<Views>(branches)...};
    }
}

#endif