/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_COMPOSE_HPP_INCLUDED
#define BOOST_NIJI_COMPOSE_HPP_INCLUDED

#include <type_traits>
#include <boost/niji/iterate.hpp>
#include <boost/niji/support/common_point.hpp>
#include <boost/niji/support/utility/pack.hpp>

namespace boost { namespace niji
{
    template<class... Paths>
    struct composite
    {
        using point_type =
            common_point_t<path_point_t<std::decay_t<Paths>>...>;

        pack<Paths...> paths;

        composite(Paths&&... paths)
          : paths(std::forward<Paths>(paths)...)
        {}

        template<class Sink>
        void iterate(Sink& sink) const
        {
            paths(pack_for_each([&sink](auto const& path)
            {
                niji::iterate(path, sink);
            }));
        }
        
        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            paths(pack_for_each([&sink](auto const& path)
            {
                niji::reverse_iterate(path, sink);
            }));
        }
    };
    
    template<class... Paths>
    inline composite<Paths...> compose(Paths&&... paths)
    {
        return {std::forward<Paths>(paths)...};
    }
}}

#endif
