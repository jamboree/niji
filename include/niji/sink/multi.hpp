/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompmultiing
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_MULTI_HPP_INCLUDED
#define NIJI_SINK_MULTI_HPP_INCLUDED

#include <tuple>
#include <type_traits>

namespace niji
{
    template<class... Sinks>
    struct multi_sink
    {
        using indices_t = std::make_index_sequence<sizeof...(Sinks)>;

        std::tuple<Sinks...> sinks;

        multi_sink(Sinks... sinks)
          : sinks(std::forward<Sinks>(sinks)...)
        {}

        template<class Tag, class... Points>
        void operator()(Tag tag, Points const&... pts) const
        {
            dispatch(tag, pts..., indices_t{});
        }
        
    private:
        template<class Tag, class... Points, std::size_t... I>
        void dispatch(Tag tag, Points const&... pts, std::index_sequence<I...>) const
        {
            bool _[] = {(std::get<I>(sinks)(tag, pts...), true)...};
            (void)_;
        }
    };

    template<class... Sinks>
    inline multi_sink<Sinks...> make_multi_sink(Sinks&&... sinks)
    {
        return {std::forward<Sinks>(sinks)...};
    }
}

#endif