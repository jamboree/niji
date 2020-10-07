/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_ANY_PATH_HPP_INCLUDED
#define NIJI_ANY_PATH_HPP_INCLUDED

#include <type_traits>
#include <functional>
#include <niji/core.hpp>
#include <niji/sink/any.hpp>

namespace niji::detail
{
    template<class Path, class Point>
    struct forward_path_holder
    {
        Path path;

        void operator()(any_sink<Point>& sink, bool) const
        {
            niji::iterate(path, sink);
        }
    };

    template<class Path, class Point>
    struct bidirectional_path_holder
    {
        Path path;

        void operator()(any_sink<Point>& sink, bool positive) const
        {
            if (positive)
                niji::iterate(path, sink);
            else
                niji::reverse_iterate(path, sink);
        }
    };
}

namespace niji
{
    template<Point Pt>
    struct any_forward_path
    {
        using point_type = Pt;

        any_forward_path() noexcept : _f([](any_sink<Pt>&, bool) {}) {}

        template<Path P> requires (!std::is_base_of_v<any_forward_path, P>)
         any_forward_path(P path)
            : _f(detail::forward_path_holder<P, Pt>{std::move(path)})
        {}

        template<Path P>
        any_forward_path(std::reference_wrapper<P> path)
            : _f(detail::forward_path_holder<P&, Pt>{path})
        {}

        void iterate(any_sink<Pt> sink) const
        {
            _f(sink, true);
        }

    protected:
        template<class Holder>
        any_forward_path(int, Holder&& f)
            : _f(std::move(f))
        {}

        std::function<void(any_sink<Pt>&, bool)> _f;
    };

    template<Point Pt>
    struct any_bidirectional_path : any_forward_path<Pt>
    {
        any_bidirectional_path() = default;

        template<BiPath P>
        any_bidirectional_path(P path)
          : any_forward_path<Pt>(0, detail::bidirectional_path_holder<P, Pt>{std::move(path)})
        {}
        
        template<BiPath P>
        any_bidirectional_path(std::reference_wrapper<P> path)
          : any_forward_path<Pt>(0, detail::bidirectional_path_holder<P&, Pt>{path})
        {}

        void reverse_iterate(any_sink<Pt> sink) const
        {
            this->_f(sink, false);
        }
    };
}

#endif