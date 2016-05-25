/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_OFFSET_HPP_INCLUDED
#define NIJI_VIEW_OFFSET_HPP_INCLUDED

#include <boost/assert.hpp>
#include <niji/support/view.hpp>
#include <niji/support/just.hpp>
#include <niji/view/stroke/detail.hpp>
#include <niji/view/stroke/join_style.hpp>
#include <niji/view/stroke/cap_style.hpp>

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
                _stroker.move_to(pt);
            }

            void operator()(line_to_t, point<T> const& pt)
            {
                _stroker.line_to(pt);
            }

            void operator()(quad_to_t, point<T> const& pt1, point<T> const& pt2)
            {
                _stroker.quad_to(pt1, pt2);
            }

            void operator()(cubic_to_t, point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
            {
                _stroker.cubic_to(pt1, pt2, pt3);
            }
            
            void operator()(end_poly_t)
            {
                _stroker.close(true); // TODO
                _stroker.finish(_sink, _reversed);
            }
            
            void operator()(end_line_t)
            {
                _stroker.finish(_sink, _reversed);
            }

            Sink& _sink;
            detail::stroker<T, Joiner, detail::non_capper> _stroker;
            bool _reversed;
        };
        
        template<class Path, class Sink>
        void render(Path const& path, Sink& sink) const
        {
            using adaptor_t = adaptor<Sink>;
            if (r)
                niji::render(path, adaptor_t{sink, {r, joiner, {}}, false});
        }

        template<class Path, class Sink>
        void inverse_render(Path const& path, Sink& sink) const
        {
            using adaptor_t = adaptor<Sink>;
            if (r)
                niji::render(path, adaptor_t{sink, {r, joiner, {}}, true});
        }
    };

    template<class Path, class T, class Joiner>
    struct path_cache<path_adaptor<Path, offset_view<T, Joiner>>>
      : default_path_cache<path_adaptor<Path, offset_view<T, Joiner>>>
    {};
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