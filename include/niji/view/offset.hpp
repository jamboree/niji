/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

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
            void move_to(point<T> const& pt)
            {
                _outline.move_to(pt);
            }

            void line_to(point<T> const& pt)
            {
                _outline.line_to(pt);
            }

            void quad_to(point<T> const& pt1, point<T> const& pt2)
            {
                _outline.quad_to(pt1, pt2);
            }

            void cubic_to(point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
            {
                _outline.cubic_to(pt1, pt2, pt3);
            }
            
            void end_closed()
            {
                _outline.close(true); // TODO
                _outline.finish(_sink, _reversed);
            }
            
            void end_open()
            {
                _outline.finish(_sink, _reversed);
            }

            Sink& _sink;
            detail::offset_outline<T, Joiner> _outline;
            bool _reversed;
        };
        
        template<Path P, class Sink>
        void iterate(P const& path, Sink& sink) const
        {
            using adaptor_t = adaptor<Sink>;
            if (r)
                niji::iterate(path, adaptor_t{sink, {r, joiner}, false});
        }

        template<Path P, class Sink>
        void reverse_iterate(P const& path, Sink& sink) const
        {
            using adaptor_t = adaptor<Sink>;
            if (r)
                niji::iterate(path, adaptor_t{sink, {r, joiner, {}}, true});
        }
    };
}

namespace niji::views
{
    template<class T, class Joiner = join_styles::bevel>
    inline offset_view<T, Joiner>
    offset(just_t<T> r, Joiner&& j = {})
    {
        return {r, std::forward<Joiner>(j)};
    }
}

#endif