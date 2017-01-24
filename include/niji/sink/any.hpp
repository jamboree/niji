/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_ANY_HPP_INCLUDED
#define NIJI_SINK_ANY_HPP_INCLUDED

#include <niji/support/command.hpp>

namespace niji { namespace any_sink_detail
{
    template<class... Ts>
    struct vtable
    {
        vtable(...) {}
    };
    
    template<class T, class... Ts>
    struct vtable<T, Ts...> : vtable<Ts...>
    {
        template<class Gen>
        vtable(Gen gen) : vtable<Ts...>(gen), f(Gen::f) {}
    
        operator T*() const { return f; }
        
        T* f;
    };
    
    template<class F>
    struct vgen
    {
        template<class... Ts>
        static void f(void* p, Ts... ts)
        {
            (*static_cast<F*>(p))(std::forward<Ts>(ts)...);
        }
    };
}}

namespace niji
{
    template<class Point>
    struct any_sink
    {
        template<class Sink>
        any_sink(Sink& sink)
          : _sink(&sink)
          , _f(any_sink_detail::vgen<Sink>())
        {}

        template<class Tag, class... Points>
        void operator()(Tag tag, Points const&... pts) const
        {
            _f(_sink, tag, pts...);
        }
        
        void* _sink;
        any_sink_detail::vtable
        <
            void(void*, move_to_t, Point const&)
          , void(void*, line_to_t, Point const&)
          , void(void*, quad_to_t, Point const&, Point const&)
          , void(void*, cubic_to_t, Point const&, Point const&, Point const&)
          , void(void*, end_line_t)
          , void(void*, end_poly_t)
        > _f;
    };
}

#endif