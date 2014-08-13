/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_VIEW_STROKE_JOIN_STYLE_HPP_INCLUDED
#define BOOST_NIJI_VIEW_STROKE_JOIN_STYLE_HPP_INCLUDED

#include <functional>
#include <boost/niji/path.hpp>
#include <boost/niji/support/point.hpp>
#include <boost/niji/support/vector.hpp>
#include <boost/niji/support/bezier/basics.hpp>

namespace boost { namespace niji { namespace detail
{
    template<class Path, class Point, class Vector, class T>
    inline
    void handle_inner_join(Path& inner, Point const& pt, Vector const& former_normal, Vector const& later_normal, T magnitude)
    {
#ifndef JAMBOREE
        inner.join(pt); // See note from SkStrokerPriv.cpp:88
        inner.join(pt - later_normal);
#else
        typedef typename Vector::coordinate_type coord_t;
        coord_t num = vectors::norm_square(former_normal),
                den = vectors::dot(former_normal, vectors::normal_cw(later_normal));
        Vector v(vectors::normal_cw(former_normal - later_normal) * num / den);
        if (magnitude < vectors::norm_square(v - former_normal))
        {
            inner.join(pt - former_normal);
            inner.join(pt);
            inner.join(pt - later_normal);
        }
        else
            inner.join(pt + v);
#endif
    }
    
    template<class T>
    using join_style_fn = std::function<void(
            path<point<T> >&, path<point<T> >&, point<T> const&
          , vector<T> const&, vector<T> const&, T, bool, bool, T)>;
}}}

namespace boost { namespace niji { namespace join_styles
{
    struct bevel
    {
        template<class T>
        void operator()
        (
            path<point<T> >& outer, path<point<T> >& inner, point<T> const& pt
          , vector<T> former_normal, vector<T> later_normal
          , T r, bool prev_is_line, bool curr_is_line, T magnitude
        ) const
        {
            path<point<T> > *o = &outer, *i = &inner;
            if (!vectors::is_ccw(former_normal, later_normal))
            {
                o = &inner, i = &outer;
                former_normal = -former_normal;
                later_normal = -later_normal;
            }
#if defined(JAMBOREE)
            o->join(pt + former_normal);
#endif
            o->join(pt + later_normal);
            detail::handle_inner_join(*i, pt, former_normal, later_normal, magnitude);
        }
    };

    struct round
    {
        template<class T>
        void operator()
        (
            path<point<T> >& outer, path<point<T> >& inner, point<T> const& pt
          , vector<T> former_normal, vector<T> later_normal
          , T r, bool prev_is_line, bool curr_is_line, T magnitude
        ) const
        {
            path<point<T> > *o = &outer, *i = &inner;
            bool is_ccw = vectors::is_ccw(former_normal, later_normal);
            if (!is_ccw)
            {
                o = &inner, i = &outer;
                former_normal = -former_normal;
                later_normal = -later_normal;
            }
            point<T> pts[17];
            transforms::affine<T> affine;
            affine.scale(r).translate(pt.x, pt.y);
            // ignore the 1st point which should be already in path 
            point<T> *it = pts + 1, *end = bezier::build_quad_arc(former_normal / r, later_normal / r, is_ccw, &affine, pts);
            for ( ; it != end; it += 2)
                o->unsafe_quad_to(*it, it[1]);

            detail::handle_inner_join(*i, pt, former_normal, later_normal, magnitude);
        }
    };
}}}

namespace boost { namespace niji
{
    template<class T>
    struct join_style : detail::join_style_fn<T>
    {
        template<class Joiner = join_styles::bevel>
        join_style(Joiner joiner = {})
          : detail::join_style_fn<T>(joiner)
        {}
    };
}}

#endif
