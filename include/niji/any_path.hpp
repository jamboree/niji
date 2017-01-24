/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_ANY_PATH_HPP_INCLUDED
#define NIJI_ANY_PATH_HPP_INCLUDED

#include <type_traits>
#include <functional>
#include <niji/render.hpp>
#include <niji/sink/any.hpp>

namespace niji
{
    template<class Point>
    class any_path
    {
        using sink_t = any_sink<Point>;
        
        template<class Path>
        struct holder
        {
            Path path;
            
            void operator()(sink_t& sink, bool positive) const
            {
                if (positive)
                    niji::render(path, sink);
                else
                    niji::inverse_render(path, sink);
            }
        };
        
        template<class Path>
        using requires_valid = std::enable_if_t<
            !std::is_same<Path, any_path>::value
          && is_renderable<Path, sink_t>::value, bool>;
        
    public:
        
        using point_type = Point;
        
        any_path() : _f([](sink_t&, bool){}) {}

        template<class Path, requires_valid<std::decay_t<Path>> = true>
        any_path(Path&& path)
          : _f(holder<std::decay_t<Path>>{std::forward<Path>(path)})
        {}
        
        template<class Path, requires_valid<std::decay_t<Path>> = true>
        any_path(std::reference_wrapper<Path> path)
          : _f(holder<Path&>{path})
        {}

        void render(sink_t sink) const
        {
            _f(sink, true);
        }

        void inverse_render(sink_t sink) const
        {
            _f(sink, false);
        }
        
    private:
        
        std::function<void(sink_t&, bool)> _f;
    };
}

#endif