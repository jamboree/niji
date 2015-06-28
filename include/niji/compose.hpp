/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_COMPOSE_HPP_INCLUDED
#define NIJI_COMPOSE_HPP_INCLUDED

#include <tuple>
#include <type_traits>
#include <niji/render.hpp>
#include <niji/support/common_point.hpp>

namespace niji
{
    template<class... Paths>
    struct composite
    {
        using point_type =
            common_point_t<path_point_t<std::decay_t<Paths>>...>;

        using unpack_sequence = std::make_index_sequence<sizeof...(Paths)>;

        composite(Paths&&... paths)
          : paths(std::forward<Paths>(paths)...)
        {}

        template<class Sink>
        void render(Sink& sink) const
        {
            for_each([&sink](auto const& path)
            {
                niji::render(path, sink);
            }, unpack_sequence{});
        }
        
        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            for_each([&sink](auto const& path)
            {
                niji::inverse_render(path, sink);
            }, unpack_sequence{});
        }

    private:

        template<class F, std::size_t... N>
        void for_each(F&& f, std::integer_sequence<std::size_t, N...>) const
        {
            std::initializer_list<bool>{(f(std::get<N>(paths)), true)...};
        }

        std::tuple<Paths...> paths;
    };
    
    template<class... Paths>
    inline composite<Paths...> compose(Paths&&... paths)
    {
        return {std::forward<Paths>(paths)...};
    }
}

#endif