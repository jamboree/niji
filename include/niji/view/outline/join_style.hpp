/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_OUTLINE_JOIN_STYLE_HPP_INCLUDED
#define NIJI_VIEW_OUTLINE_JOIN_STYLE_HPP_INCLUDED

#include <functional>
#include <niji/path.hpp>
#include <niji/support/point.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/numbers.hpp>
#include <niji/support/numeric.hpp>
#include <niji/support/bezier.hpp>

namespace niji::detail
{
    template<class T>
    inline
    void handle_inner_join
    (
        path<point<T>>& inner, point<T> const& pt
      , vector<T> const& former_normal, vector<T> const& later_normal
      , T magnitude
    )
    {
#ifndef JAMBOREE
        //inner.join(pt); // See note from SkStrokerPriv.cpp:88
        inner.join(pt - later_normal);
#else
        T num = vectors::norm_square(former_normal),
          den = vectors::dot(former_normal, vectors::normal_cw(later_normal));
        auto v(vectors::normal_cw(former_normal - later_normal) * num / den);
        if (magnitude < vectors::norm_square(v - former_normal))
        {
            inner.join(pt - former_normal);
            //inner.join(pt);
            inner.join(pt - later_normal);
        }
        else
            inner.join(pt + v);
#endif
    }
    
    enum class angle_type
    {
        nearly180,
        sharp,
        shallow,
        nearly_line
    };
    
    template<class T>
    inline angle_type get_angle_type(T dot)
    {
        // need more precise fixed normalization
        if (dot >= 0)   // shallow or line
            return numeric::is_nearly_zero(1 - dot) ? angle_type::nearly_line : angle_type::shallow;
        else            // sharp or 180
            return numeric::is_nearly_zero(1 + dot) ? angle_type::nearly180 : angle_type::sharp;
    }
    
    template<class T>
    using join_style_fn = std::function<void(
            path<point<T>>*, path<point<T>>*, point<T> const&
          , vector<T> const&, vector<T> const&, T, bool, bool, T)>;
}

namespace niji::join_styles
{
    struct bevel
    {
        template<class T>
        void operator()
        (
            path<point<T>>* o, path<point<T>>* i, point<T> const& pt
          , vector<T> former_normal, vector<T> later_normal
          , T r, bool prev_is_line, bool curr_is_line, T magnitude
        ) const
        {
            if (!vectors::is_ccw(former_normal, later_normal))
            {
                std::swap(o, i);
                former_normal = -former_normal;
                later_normal = -later_normal;
            }
            if (o)
            {
#if defined(JAMBOREE)
                o->join(pt + former_normal);
#endif
                o->join(pt + later_normal);
            }
            if (i)
                detail::handle_inner_join(*i, pt, former_normal, later_normal, magnitude);
        }
    };

    struct round
    {
        template<class T>
        void operator()
        (
            path<point<T>>* o, path<point<T>>* i, point<T> const& pt
          , vector<T> former_normal, vector<T> later_normal
          , T r, bool prev_is_line, bool curr_is_line, T magnitude
        ) const
        {
            bool is_ccw = vectors::is_ccw(former_normal, later_normal);
            if (!is_ccw)
            {
                std::swap(o, i);
                former_normal = -former_normal;
                later_normal = -later_normal;
            }
            transforms::affine<T> affine;
            affine.scale(r).translate(pt.x, pt.y);
#   if defined(NIJI_NO_CUBIC_APPROX)
            point<T> pts[17];
            // ignore the 1st point which should be already in path
            auto it = pts + 1, end = bezier::build_quad_arc(former_normal / r, later_normal / r, is_ccw, &affine, pts);
#   else
            point<T> pts[13];
            // ignore the 1st point which should be already in path
            auto it = pts + 1, end = bezier::build_cubic_arc(former_normal / r, later_normal / r, is_ccw, &affine, pts);
#   endif
            if (o)
            {
#   if defined(JAMBOREE)
                o->join(*pts);
#   endif
#   if defined(NIJI_NO_CUBIC_APPROX)
                for (; it != end; it += 2)
                    o->unsafe_quad_to(*it, it[1]);
#   else
                for (; it != end; it += 3)
                    o->unsafe_cubic_to(*it, it[1], it[2]);
#   endif
            }
            if (i)
                detail::handle_inner_join(*i, pt, former_normal, later_normal, magnitude);
        }
    };

    template<class T>
    struct miter
    {
        T inv_limit;
        
        miter() : inv_limit(T(1) / T(4)) {}

        explicit miter(T limit)
          : inv_limit(limit > 1? 1 / limit : 1)
        {}

        void operator()
        (
            path<point<T>>* o, path<point<T>>* i, point<T> const& pt
          , vector<T> former_normal, vector<T> later_normal
          , T r, bool prev_is_line, bool curr_is_line, T magnitude
        ) const
        {
            using detail::angle_type;
            
            T dot = vectors::dot(former_normal, later_normal) / (r * r);
            angle_type a = detail::get_angle_type(dot);
            
            if (a == angle_type::nearly_line)
                return;
            
            vector<T> mid;
            auto do_miter = [&](bool do_miter)
            {
                if (o)
                {
#if defined(JAMBOREE)
                    o->join(pt + former_normal);
#endif
                    switch (do_miter)
                    {
                    case true:
                        if (prev_is_line)
                            o->back() = pt + mid;
                        else
                            o->join(pt + mid);
                        if (!curr_is_line)
                    default: // do bevel
                        o->join(pt + later_normal);
                    }
                }
                if (i)
                    detail::handle_inner_join(*i, pt, former_normal, later_normal, magnitude);
            };
            
            if (a == angle_type::nearly180)
                return do_miter(false);

            bool is_ccw = vectors::is_ccw(former_normal, later_normal);
            if (!is_ccw)
            {
                std::swap(o, i);
                former_normal = -former_normal;
                later_normal = -later_normal;
            }
            
            // Before we enter the world of square-roots and divides,
            // check if we're trying to join an upright right angle
            // (common case for stroking rectangles). If so, special case
            // that (for speed an accuracy).
            // Note: we only need to check one normal if dot==0
            if (0 == dot && inv_limit <= numbers::one_div_root_two<T>)
            {
                mid = former_normal + later_normal;
                return do_miter(true);
            }
            
            T sin_half_theta = sqrt((1 + dot) / 2);
            if (sin_half_theta < inv_limit)
                return do_miter(false);

            mid = former_normal + later_normal;
            mid = mid * r / (sin_half_theta * vectors::norm(mid));
            do_miter(true);
        }
    };
}

namespace niji
{
    template<class T>
    struct join_style : detail::join_style_fn<T>
    {
        template<class Joiner = join_styles::bevel>
        join_style(Joiner joiner = {})
          : detail::join_style_fn<T>(joiner)
        {}
    };
}

#endif