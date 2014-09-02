/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_ITERATE_HPP_INCLUDED
#define BOOST_NIJI_ITERATE_HPP_INCLUDED

#include <type_traits>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/stoppable_sink.hpp>

namespace boost { namespace niji { namespace detail
{
    template<class Sink>
    inline auto wrap_stoppable_sink(Sink& sink)
    {
        return [&sink](auto&&... ts)
        {
            sink(static_cast<decltype(ts)>(ts)...);
        };
    }
    
    template<class Path, class Sink>
    std::enable_if_t<std::is_base_of<stoppable_sink, Sink>::value>
    iterate_impl(Path const& path, Sink& sink)
    {
        auto wrapped = wrap_stoppable_sink(sink);
        try
        {
            path_iterate<Path, decltype(wrapped)>::apply(path, wrapped);
        }
        catch (iteration_stopped&) {}
    }
    
    template<class Path, class Sink>
    inline std::enable_if_t<!std::is_base_of<stoppable_sink, Sink>::value>
    iterate_impl(Path const& path, Sink& sink)
    {
        path_iterate<Path, Sink>::apply(path, sink);
    }
    
    template<class Path, class Sink>
    std::enable_if_t<std::is_base_of<stoppable_sink, Sink>::value>
    reverse_iterate_impl(Path const& path, Sink& sink)
    {
        auto wrapped = wrap_stoppable_sink(sink);
        try
        {
            path_reverse_iterate<Path, decltype(wrapped)>::apply(path, wrapped);
        }
        catch (iteration_stopped&) {}
    }
    
    template<class Path, class Sink>
    inline std::enable_if_t<!std::is_base_of<stoppable_sink, Sink>::value>
    reverse_iterate_impl(Path const& path, Sink& sink)
    {
        path_reverse_iterate<Path, Sink>::apply(path, sink);
    }
}}}

namespace boost { namespace niji
{
    template<class Path, class Sink>
    inline void iterate(Path const& path, Sink&& sink)
    {
        detail::iterate_impl(path, sink);
    }
    
    template<class Path, class Sink>
    inline void reverse_iterate(Path const& path, Sink&& sink)
    {
        detail::reverse_iterate_impl(path, sink);
    }
}}

#endif
