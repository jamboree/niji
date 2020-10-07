/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_MAKE_COMPOSITE_HPP_INCLUDED
#define NIJI_MAKE_COMPOSITE_HPP_INCLUDED

#include <tuple>
#include <type_traits>
#include <niji/core.hpp>

namespace niji
{
    template<class... Paths>
    struct composite_path
    {
        using point_type =
            std::common_type_t<path_point_t<std::decay_t<Paths>>...>;

        using unpack_sequence = std::make_index_sequence<sizeof...(Paths)>;

        composite_path(Paths&&... paths)
          : paths(std::forward<Paths>(paths)...)
        {}

        template<class Sink>
        void iterate(Sink& sink) const
        {
            iterate_impl(niji::iterate, sink, unpack_sequence{});
        }
        
        template<class Sink> requires (BiPath<std::decay_t<Paths>> && ...)
        void reverse_iterate(Sink& sink) const
        {
            iterate_impl(niji::reverse_iterate, sink, unpack_sequence{});
        }

    private:
        template<class F, class Sink, std::size_t... N>
        void iterate_impl(F f, Sink& sink, std::integer_sequence<std::size_t, N...>) const
        {
            (f(std::get<N>(paths), sink), ...);
        }

        std::tuple<Paths...> paths;
    };
    
    template<class... Paths>
    inline composite_path<Paths...> make_composite(Paths&&... paths)
    {
        return {std::forward<Paths>(paths)...};
    }
}

#endif