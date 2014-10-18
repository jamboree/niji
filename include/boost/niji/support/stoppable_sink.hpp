/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_STOPPABLE_SINK_HPP_INCLUDED
#define BOOST_NIJI_STOPPABLE_SINK_HPP_INCLUDED

#include <type_traits>
#include <boost/niji/detail/iterate.hpp>

namespace boost { namespace niji
{
    struct iteration_stopped {};
    
    class stoppable_sink
    {
    protected:
        
        static void stop()
        {
            throw iteration_stopped();
        }
    };
}}

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

    template<template<class, class, class> class Iterate, class Path, class Sink>
    struct iterate_impl<Iterate, Path, Sink, std::enable_if_t<std::is_base_of<stoppable_sink, Sink>::value>>
    {
        static void apply(Path const& path, Sink& sink)
        {
            auto wrapped = wrap_stoppable_sink(sink);
            try
            {
                Iterate<Path, decltype(wrapped), void>::apply(path, wrapped);
            }
            catch (iteration_stopped&) {}
        }
    };
}}}

#endif
