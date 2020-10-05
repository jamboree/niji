/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_STROKE_HPP_INCLUDED
#define NIJI_VIEW_STROKE_HPP_INCLUDED

#include <type_traits>
#include <niji/support/view.hpp>
#include <niji/support/just.hpp>
#include <niji/view/detail/stroker.hpp>
#include <niji/view/outline/join_style.hpp>
#include <niji/view/outline/cap_style.hpp>

namespace niji
{
    template<class T, class Joiner = join_style<T>, class Capper = cap_style<T>>
    struct stroke_view : view<stroke_view<T, Joiner, Capper>>
    {
        template<class Path>
        using point_type = point<T>;
        
        T radius;
        Joiner joiner;
        Capper capper;
        
        stroke_view() : radius() {}

        stroke_view(T radius, Joiner joiner, Capper capper)
          : radius(radius)
          , joiner(std::forward<Joiner>(joiner))
          , capper(std::forward<Capper>(capper))
        {}

        template<class Sink>
        struct adaptor
        {
            void move_to(point<T> const& pt)
            {
                _stroker.move_to(pt);
            }

            void line_to(point<T> const& pt)
            {
                _stroker.line_to(pt);
            }

            void quad_to(point<T> const& pt1, point<T> const& pt2)
            {
                _stroker.quad_to(pt1, pt2);
            }

            void cubic_to(point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
            {
                _stroker.cubic_to(pt1, pt2, pt3);
            }
            
            void end_closed()
            {
                _stroker.close(true); // TODO
                _stroker.finish(_sink, _reversed);
            }
            
            void end_open()
            {
                _stroker.cut(true); // TODO
                _stroker.finish(_sink, _reversed);
            }

            Sink& _sink;
            detail::stroker<T, std::decay_t<Joiner>, std::decay_t<Capper>> _stroker;
            bool _reversed;
        };
        
        template<class Path, class Sink>
        void iterate(Path const& path, Sink& sink) const
        {
            using adaptor_t = adaptor<Sink>;
            if (radius)
                niji::iterate(path, adaptor_t{sink, {radius, joiner, capper}, false});
        }

        template<class Path, class Sink>
        void reverse_iterate(Path const& path, Sink& sink) const
        {
            using adaptor_t = adaptor<Sink>;
            if (radius)
                niji::iterate(path, adaptor_t{sink, {radius, joiner, capper}, true});
        }
    };
}

namespace niji::views
{
    template<class T, class Joiner = join_styles::bevel, class Capper = cap_styles::butt>
    inline stroke_view<T, Joiner, Capper>
    stroke(just_t<T> radius, Joiner&& j = {}, Capper&& c = {})
    {
        return {radius, std::forward<Joiner>(j), std::forward<Capper>(c)};
    }
}

#endif