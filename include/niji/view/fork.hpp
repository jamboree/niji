/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_FORK_HPP_INCLUDED
#define NIJI_VIEW_FORK_HPP_INCLUDED

#include <tuple>
#include <type_traits>
#include <niji/render.hpp>
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

        template<class Path, class Sink>
        void render(Path const& path, Sink& sink) const
        {
            render_impl(niji::render, path, sink, indices_t{});
        }

        template<class Path, class Sink>
        void inverse_render(Path const& path, Sink& sink) const
        {
            render_impl(niji::inverse_render, path, sink, indices_t{});
        }

    private:
        template<class F, class Path, class Sink, std::size_t... I>
        void render_impl(F f, Path const& path, Sink& sink, std::index_sequence<I...>) const
        {
            f(path, sink);
            bool _[] = {(f(path | std::get<I>(branches), sink), true)...};
            (void)_;
        }
    };
}

namespace niji { namespace views
{
    template<class... Views>
    inline fork_view<Views...> fork(Views&&... branches)
    {
        return {std::forward<Views>(branches)...};
    }
}}

#endif