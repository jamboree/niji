/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_ITERATE_HPP_INCLUDED
#define BOOST_NIJI_ITERATE_HPP_INCLUDED

#include <type_traits>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/detail/iterate.hpp>

namespace boost { namespace niji
{
    template<class Path, class Sink>
    inline void iterate(Path const& path, Sink&& sink)
    {
        using sink_t = std::remove_reference_t<Sink>;
        detail::iterate_impl<path_iterate, Path, sink_t>::apply(path, sink);
    }
    
    template<class Path, class Sink>
    inline void reverse_iterate(Path const& path, Sink&& sink)
    {
        using sink_t = std::remove_reference_t<Sink>;
        detail::iterate_impl<path_reverse_iterate, Path, sink_t>(path, sink);
    }
}}

#endif
