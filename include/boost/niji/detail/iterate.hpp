/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_DETAIL_ITERATE_HPP_INCLUDED
#define BOOST_NIJI_DETAIL_ITERATE_HPP_INCLUDED

namespace boost { namespace niji { namespace detail
{
    template<template<class, class, class> class Iterate, class Path, class Sink, class = void>
    struct iterate_impl
    {
        static void apply(Path const& path, Sink& sink)
        {
            Iterate<Path, Sink, void>::apply(path, sink);
        }
    };
}}}

#endif
