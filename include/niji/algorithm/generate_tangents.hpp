/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_ALGORITHM_GENERATE_TANGENTS_HPP_INCLUDED
#define NIJI_ALGORITHM_GENERATE_TANGENTS_HPP_INCLUDED

#include <niji/render.hpp>
#include <niji/support/command.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/point.hpp>
#include <niji/support/bezier.hpp>

namespace niji { namespace detail
{
    template<class T, class F>
    struct tangents_sink
    {
        using point_t = point<T>;
        using vector_t = vector<T>;

        tangents_sink(T step, T offset, F& f) : _step(step), _offset(), _f(f)
        {
            using std::fmod;

            offset = fmod(offset, _step);
            if (offset < 0)
                offset += _step;
            _offset = _step - offset;
        }

        void operator()(move_to_t, point_t const& pt)
        {
            _first_pt = _prev_pt = pt;
        }

        void operator()(line_to_t, point_t const& pt)
        {
            vector_t v(pt - _prev_pt);
            T len(vectors::norm(v));
            do_act(len, [&, u = v / len](T sum, T len)
            {
                _f(_prev_pt + u * sum, u);
            });
            _prev_pt = pt;
        }

        void operator()(quad_to_t, point_t const& pt1, point_t const& pt2)
        {
            point_t pts[3] = {_prev_pt, pt1, pt2};
            point_t chops[5];
            T len(bezier::quad_length(_prev_pt, pt1, pt2));
            do_act(len, [&](T sum, T len)
            {
                bezier::curve_bisect(NIJI_MAX_QUAD_SUBDIVIDE, sum, len, [&](T t)
                {
                    bezier::chop_quad_at(pts, chops, t);
                    return bezier::quad_length(chops[0], chops[1], chops[2]);
                });
                _f(chops[2], vectors::unit(chops[2] - chops[1]));
            });
            _prev_pt = pt2;
        }

        void operator()(cubic_to_t, point_t const& pt1, point_t const& pt2, point_t const& pt3)
        {
            point_t pts[4] = {_prev_pt, pt1, pt2, pt3};
            point_t chops[7];
            T len(bezier::cubic_length(_prev_pt, pt1, pt2, pt3));
            do_act(len, [&](T sum, T len)
            {
                bezier::curve_bisect(NIJI_MAX_CUBIC_SUBDIVIDE, sum, len, [&](T t)
                {
                    bezier::chop_cubic_at(pts, chops, t);
                    return bezier::cubic_length(chops[0], chops[1], chops[2], chops[3]);
                });
                _f(chops[3], vectors::unit(chops[3] - chops[2]));
            });
            _prev_pt = pt3;
        }
        
        void operator()(end_open_t) {}

        void operator()(end_closed_t)
        {
            operator()(line_to_t{}, _first_pt);
        }
        
    private:
        template<class Actor>
        void do_act(T len, Actor&& act)
        {
            if (_offset > len)
            {
                _offset -= len;
                return;
            }
            act(_offset, len);
            T sum = _offset;
            _offset = 0;
            for ( ; ; )
            {
                sum += _step;
                if (sum > len)
                {
                    _offset = sum - len;
                    return;
                }
                act(sum, len);
            }
        }

        T _step;
        T _offset;
        F& _f;
        point_t _prev_pt, _first_pt;
    };
}}

namespace niji
{
    template<class Path, class T, class F>
    void generate_tangents(Path const& path, T const& step, T const& offset, F&& f)
    {
        using coord_t = path_coordinate_t<Path>;
        detail::tangents_sink<coord_t, F> sink(step, offset, f);
        niji::render(path, sink);
    }
}

#endif