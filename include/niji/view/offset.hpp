/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_OFFSET_HPP_INCLUDED
#define NIJI_VIEW_OFFSET_HPP_INCLUDED

#include <boost/assert.hpp>
#include <niji/support/view.hpp>
#include <niji/support/just.hpp>
#include <niji/view/detail/offset_outline.hpp>
#include <niji/view/outline/join_style.hpp>

namespace niji
{
    template<class T, class Joiner>
    struct offset_view : view<offset_view<T, Joiner>>
    {
        template<class Path>
        using point_type = point<T>;
        
        T r;
        Joiner joiner;

        offset_view() : r() {}

        offset_view(T r, Joiner joiner)
          : r(r)
          , joiner(std::forward<Joiner>(joiner))
        {}

        template<class Sink>
        struct adaptor
        {
            void operator()(move_to_t, point<T> const& pt)
            {
                _outline.move_to(pt);
            }

            void operator()(line_to_t, point<T> const& pt)
            {
                _outline.line_to(pt);
            }

            void operator()(quad_to_t, point<T> const& pt1, point<T> const& pt2)
            {
                _outline.quad_to(pt1, pt2);
            }

            void operator()(cubic_to_t, point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
            {
                _outline.cubic_to(pt1, pt2, pt3);
            }
            
            void operator()(end_closed_t)
            {
                _outline.close(true); // TODO
                _outline.finish(_sink, _reversed);
            }
            
            void operator()(end_open_t)
            {
                _outline.finish(_sink, _reversed);
            }

            Sink& _sink;
            detail::offset_outline<T, Joiner> _outline;
            bool _reversed;
        };
        
        template<class Path, class Sink>
        void render(Path const& path, Sink& sink) const
        {
            using adaptor_t = adaptor<Sink>;
            if (r)
                niji::render(path, adaptor_t{sink, {r, joiner}, false});
        }

        template<class Path, class Sink>
        void inverse_render(Path const& path, Sink& sink) const
        {
            using adaptor_t = adaptor<Sink>;
            if (r)
                niji::render(path, adaptor_t{sink, {r, joiner, {}}, true});
        }
    };
}

namespace niji { namespace views
{
    template<class T, class Joiner = join_styles::bevel>
    inline offset_view<T, Joiner>
    offset(just_t<T> r, Joiner&& j = {})
    {
        return {r, std::forward<Joiner>(j)};
    }
}}

#endif