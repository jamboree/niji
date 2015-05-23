/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_PATH_REF_HPP_INCLUDED
#define NIJI_PATH_REF_HPP_INCLUDED

#include <type_traits>
#include <niji/render.hpp>
#include <niji/detail/any_sink.hpp>

namespace niji
{
    template<class Point>
    class path_ref
    {
        using sink_t = detail::any_sink<Point>;
        
        template<class Path>
        using requires_valid = std::enable_if_t<
            !std::is_same<Path, path_ref>::value
          && is_renderable<Path, sink_t>::value, bool>;
        
        template<class Path>
        static void f(void const* p, sink_t& sink, bool positive)
        {
            auto& path = *static_cast<Path const*>(p);
            if (positive)
                niji::render(path, sink);
            else
                niji::inverse_render(path, sink);
        }
        
    public:
        
        using point_type = Point;

        template<class Path, requires_valid<Path> = true>
        path_ref(Path const& path)
          : _p(&path), _f(f<Path>)
        {}

        void render(sink_t sink) const
        {
            _f(_p, sink, true);
        }

        void inverse_render(sink_t sink) const
        {
            _f(_p, sink, false);
        }
        
    private:
        
        void const* _p;
        void(*_f)(void const*, sink_t&, bool);
    };
}

#endif
