/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_VIEW_STROKE_HPP_INCLUDED
#define BOOST_NIJI_VIEW_STROKE_HPP_INCLUDED

#include <type_traits>
#include <boost/niji/support/view.hpp>
#include <boost/niji/support/utility/as.hpp>
#include <boost/niji/view/stroke/detail.hpp>
#include <boost/niji/view/stroke/join_style.hpp>
#include <boost/niji/view/stroke/cap_style.hpp>

namespace boost { namespace niji
{
    template<class T, class Joiner = join_style<T>, class Capper = cap_style<T>>
    struct stroke_view : view<stroke_view<T, Joiner, Capper>>
    {
        template<class Path>
        using point_type = point<T>;
        
        T r;
        Joiner joiner;
        Capper capper;
        
        stroke_view() : r() {}

        stroke_view(T r, Joiner joiner, Capper capper)
          : r(r)
          , joiner(std::forward<Joiner>(joiner))
          , capper(std::forward<Capper>(capper))
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
            if (r)
                niji::iterate(path, adaptor_t{sink, {r, joiner, capper}, false});
        }

        template<class Path, class Sink>
        void reverse_iterate(Path const& path, Sink& sink) const
        {
            using adaptor_t = adaptor<Sink>;
            if (r)
                niji::iterate(path, adaptor_t{sink, {r, joiner, capper}, true});
        }
    };

    template<class Path, class T, class Joiner, class Capper>
    struct path_cache<path_view<Path, stroke_view<T, Joiner, Capper>>>
      : default_path_cache<path_view<Path, stroke_view<T, Joiner, Capper>>>
    {};
}}

namespace boost { namespace niji { namespace views
{
    template<class T, class Joiner = join_styles::bevel, class Capper = cap_styles::butt>
    inline stroke_view<T, holder_t<Joiner>, holder_t<Capper>>
    stroke(as_t<T> r, Joiner&& j = {}, Capper&& c = {})
    {
        return {r, std::forward<Joiner>(j), std::forward<Capper>(c)};
    }
}}}

#endif
