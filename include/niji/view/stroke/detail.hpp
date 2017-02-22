/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_STROKE_DETAIL_HPP_INCLUDED
#define NIJI_VIEW_STROKE_DETAIL_HPP_INCLUDED

#include <niji/path.hpp>
#include <niji/support/command.hpp>
#include <niji/support/point.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/bezier.hpp>

namespace niji { namespace detail
{
    struct non_capper
    {
        template<class... Ts>
        void operator()(Ts&&...) const {}
    };
    
    // This is used for both stroke & offset. If Capper == non_capper, it's for
    // offset and we don't need to deal with inner path.
    template<class T, class Joiner, class Capper>
    struct stroker
    {
        using point_t = point<T>;
        using vector_t = vector<T>;
        using path_t = path<point_t>;

        static constexpr bool do_inner =
            !std::is_same<Capper, non_capper>::value;

        Joiner const& _join;
        Capper const& _cap;

        T _r, _pre_magnitude, _first_magnitude;
        int _seg_count;
        path_t _outer, _inner/*, extra*/;
        point_t _prev_pt, _first_pt/*, first_outer_pt*/;
        vector_t _prev_normal, _first_normal;
        bool _prev_is_line;

        stroker(T r, Joiner const& join, Capper const& cap)
          : _join(join), _cap(cap)
          , _r(r), _pre_magnitude(), _first_magnitude()
          , _seg_count(), _prev_is_line()
        {}

        void move_to(point_t const& pt)
        {
            if (_seg_count)
                cut(false);
            _seg_count = 0;
            _first_pt = _prev_pt = pt;
        }

        void line_to(point_t const& pt)
        {
            if (vectors::is_degenerated(pt - _prev_pt))
            {
                _seg_count -= !_seg_count;
                return;
            }
            vector_t normal;
            pre_join(pt, normal, true);
            line_to_stroke(pt, normal);
            post_join(pt, normal);
        }

        void quad_to(point_t const& pt1, point_t const& pt2)
        {
            bool degenerateAB = vectors::is_degenerated(pt1 - _prev_pt);
            bool degenerateBC = vectors::is_degenerated(pt2 - pt1);

            if (degenerateAB | degenerateBC)
            {
                if (degenerateAB ^ degenerateBC)
                    line_to(pt2);
                return;
            }
            vector_t normalAB, normalBC;
            pre_join(pt1, normalAB, false);
            {
                point_t pts[3] = {_prev_pt, pt1, pt2}, tmp[5];
                if (bezier::chop_quad_at_max_curvature(pts, tmp))
                {
                    normalBC = vectors::normal_cw(pts[2] - pts[1]);
                    normalBC = normalBC * _r / vectors::norm(normalBC);
                    if (vectors::too_pinchy(normalAB, normalBC))
                    {
                        _outer.join(tmp[2] + normalAB);
                        _outer.join(tmp[2] + normalBC);
                        _outer.join(tmp[4] + normalBC);
                        
                        if (do_inner)
                        {
                            _inner.join(tmp[2] - normalAB);
                            _inner.join(tmp[2] - normalBC);
                            _inner.join(tmp[4] - normalBC);
                            // Add circle???                   
                        }
                    }
                    else
                    {
                        quad_to_stroke(tmp, normalAB, normalBC, NIJI_MAX_QUAD_SUBDIVIDE);
                        vector_t normal(normalBC);
                        quad_to_stroke(tmp + 2, normal, normalBC, NIJI_MAX_QUAD_SUBDIVIDE);
                    }
                }
                else
                    quad_to_stroke(pts, normalAB, normalBC, NIJI_MAX_QUAD_SUBDIVIDE);
            }
            post_join(pt2, normalBC);
        }
        
        void cubic_to(point_t const& pt1, point_t const& pt2, point_t const& pt3)
        {
            bool degenerateAB = vectors::is_degenerated(pt1 - _prev_pt);
            bool degenerateBC = vectors::is_degenerated(pt2 - pt1);
            bool degenerateCD = vectors::is_degenerated(pt3 - pt2);
            
            if (degenerateAB + degenerateBC + degenerateCD >= 2)
            {
                line_to(pt3);
                return;
            }
            vector_t normalAB, normalCD;
            pre_join(degenerateAB? pt2 : pt1, normalAB, false);
            {
                point_t pts[4] = {_prev_pt, pt1, pt2, pt3}, tmp[13];
                T ts[3];
                auto count = bezier::chop_cubic_at_max_curvature(pts, tmp, ts);
                vector_t normal(normalAB);
                auto pos = tmp, end = tmp + 3 * count;
                for ( ; pos != end; pos += 3)
                {
                    cubic_to_stroke(pos, normal, normalCD, NIJI_MAX_CUBIC_SUBDIVIDE);
                    normal = normalCD;
                }
            }
            post_join(pt3, normalCD);
        }

        void cut(bool curr_is_line)
        {
            switch (_seg_count)
            {
            case -1:
                degenerated_dot();
            case 0:
                return;
            }
            // cap the end
            _cap(_outer, _prev_pt, _prev_normal, curr_is_line);
            _outer.reverse_splice(_inner);

            // cap the start
            _cap(_outer, _first_pt, -_first_normal, _prev_is_line);
            _outer.close();

            _inner.clear();
        }

        void close(bool curr_is_line)
        {
            if (_seg_count < 1)
            {
                if (_seg_count == -1)
                    degenerated_dot();
                return;
            }

            line_to(_first_pt);
            _join
            (
                _outer, _inner, _prev_pt, _prev_normal, _first_normal
              , _r, _prev_is_line, curr_is_line
              , std::min(_pre_magnitude, _first_magnitude)
            );
            _outer.close();
            if (do_inner)
            {
                _inner.close();
                _outer.reverse_splice(_inner);
                _inner.clear();
            }
        }

        void pre_join(point_t const& pt, vector_t& normal, bool curr_is_line)
        {
            using std::sqrt;
            
            normal = vectors::normal_cw(pt - _prev_pt);
            T magnitude = vectors::norm_square(normal);
            normal = normal * _r / sqrt(magnitude);

            if (_seg_count > 0) // we have a previous segment
            {
                _join
                (
                    _outer, _inner, _prev_pt, _prev_normal, normal
                    , _r, _prev_is_line, curr_is_line
                    , std::min(_pre_magnitude, magnitude)
                );
            }
            else
            {
                _first_normal = normal;
                //first_outer_pt = _prev_pt + normal;
                _first_magnitude = magnitude;
#ifdef JAMBOREE
                if (!curr_is_line)
#endif
                {
                    _outer.join(_prev_pt + normal);
                    if (do_inner)
                        _inner.join(_prev_pt - normal);
                }
            }
            _prev_is_line = curr_is_line;
            _pre_magnitude = magnitude;
        }

        void post_join(point_t const& pt, vector_t const& normal)
        {
            _prev_pt = pt;
            _prev_normal = normal;
            ++_seg_count;
        }

        void line_to_stroke(point_t const& pt, vector_t const& normal)
        {
#ifndef JAMBOREE
            _outer.join(pt + normal);
            if (do_inner)
                _inner.join(pt - normal);
#endif
        }

        void quad_to_stroke(point_t const pts[3], vector_t const& normalAB, vector_t& normalBC, int subdivide)
        {
            using std::sqrt;
            
            normalBC = vectors::normal_cw(pts[2] - pts[1]);
            if (vectors::is_degenerated(normalBC))
            {
                line_to_stroke(pts[2], normalAB);
                normalBC = normalAB;
                return;
            }
            if (subdivide-- && vectors::too_curvy(normalAB, normalBC))
            {
                point_t tmp[5];
                vector_t normal;
                bezier::chop_quad_at_half(pts, tmp);
                quad_to_stroke(tmp, normalAB, normal, subdivide);
                quad_to_stroke(tmp + 2, normal, normalBC, subdivide);
            }
            else
            {
                normalBC = normalBC * _r / vectors::norm(normalBC);
                vector_t normalB(vectors::normal_cw(pts[2] - pts[0]));
                T dot = vectors::dot(normalAB, normalBC);
                T ns = vectors::norm_square(normalB);
                T rs = _r * _r;
                normalB = normalB * _r / sqrt((dot + rs) * ns / (2 * rs));
                _outer.unsafe_quad_to(pts[1] + normalB, pts[2] + normalBC);
                if (do_inner)
                    _inner.unsafe_quad_to(pts[1] - normalB, pts[2] - normalBC);
            }
        }
        
        void cubic_to_stroke(point_t const pts[4], vector_t const& normalAB, vector_t& normalCD, int subdivide)
        {
            using std::sqrt;
            
            vector_t AB = pts[1] - pts[0];
            vector_t CD = pts[3] - pts[2];

            bool degenerateAB = vectors::is_degenerated(AB);
            bool degenerateCD = vectors::is_degenerated(CD);
            
            auto stroke = [&]
            {
                line_to_stroke(pts[3], normalAB);
                normalCD = normalAB;
            };
            
            if (degenerateAB & degenerateCD)
                return stroke();

            if (degenerateAB)
            {
                AB = pts[2] - pts[0];
                degenerateAB = vectors::is_degenerated(AB);
            }
            if (degenerateCD)
            {
                CD = pts[3] - pts[1];
                degenerateCD = vectors::is_degenerated(CD);
            }
            if (degenerateAB | degenerateCD)
                return stroke();
            
            normalCD = vectors::normal_cw(CD) * _r / vectors::norm(CD);
            vector_t normalBC(vectors::normal_cw(pts[2] - pts[1]));
            bool degenerateBC = vectors::is_degenerated(normalBC);
            
            if (degenerateBC || vectors::too_curvy(normalAB, normalBC) ||
                vectors::too_curvy(normalBC, normalCD))
            {
                if (!subdivide--)
                    return stroke();

                point_t tmp[7];
                vector_t normal, dummy;
        
                bezier::chop_cubic_at_half(pts, tmp);
                cubic_to_stroke(tmp, normalAB, normal, subdivide);
                // we use dummys since we already have a valid (and more accurate)
                // normals for CD
                cubic_to_stroke(tmp + 3, normal, dummy, subdivide);
            }
            else
            {
                normalBC = normalBC * _r / vectors::norm(normalBC);
                vector_t normalB(normalAB + normalBC), normalC(normalCD + normalBC);

                T rs = _r * _r, rs2 = 2 * rs;
                T dot = vectors::dot(normalAB, normalBC);
                T ns = vectors::norm_square(normalB);
                normalB = normalB * _r / sqrt((dot + rs) * ns / rs2);
                dot = vectors::dot(normalCD, normalBC);
                ns = vectors::norm_square(normalC);
                normalC = normalC * _r / sqrt((dot + rs) * ns / rs2);

                _outer.unsafe_cubic_to(pts[1] + normalB, pts[2] + normalC, pts[3] + normalCD);
                if (do_inner)
                    _inner.unsafe_cubic_to(pts[1] - normalB, pts[2] - normalC, pts[3] - normalCD);
            }
        }
        
        void degenerated_dot()
        {
            _cap(_outer, _prev_pt, vector_t(_r, 0), true);
            _cap(_outer, _prev_pt, vector_t(-_r, 0), true);
        }
        
        template<class Sink>
        void finish(Sink& sink, bool reversed)
        {
            if (reversed)
                _outer.inverse_render(sink);
            else
                _outer.render(sink);
            _outer.clear();
            _seg_count = 0;
        }
    };
}}

#endif