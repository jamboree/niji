/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2017-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_ALGORITHM_GENERATE_TANGENTS_HPP_INCLUDED
#define NIJI_ALGORITHM_GENERATE_TANGENTS_HPP_INCLUDED

#include <niji/core.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/point.hpp>
#include <niji/support/bezier.hpp>

namespace niji::detail
{
    enum status
    {
        empty,
        valid,
        invalid
    };

    template<class T, class Range, class F>
    struct tangents_sink
    {
        using point_t = point<T>;
        using vector_t = vector<T>;

        static_assert(std::is_signed_v<T>);

        tangents_sink(Range const& steps, F& f) : _steps(steps), _f(f)
        {
            _offset = _steps ? _steps.next() : -1;
        }

        void move_to(point_t const& pt)
        {
            _first_pt = _prev_pt = pt;
            _head_status = status::empty;
        }

        void line_to(point_t const& pt)
        {
            vector_t const v = pt - _prev_pt;
            if (step(v))
            {
                T len(vectors::norm(v));
                do_act(len, [&, u = v / len](T sum, T len)
                {
                    _f(_prev_pt + u * sum, u);
                });
            }
            update(pt, v);
        }

        void quad_to(point_t const& pt1, point_t const& pt2)
        {
            vector_t const v = pt2 - pt1;
            if (step(v))
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
            }
            update(pt2, v);
        }

        void cubic_to(point_t const& pt1, point_t const& pt2, point_t const& pt3)
        {
            vector_t const v = pt3 - pt2;
            if (step(v))
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
            }
            update(pt3, v);
        }
        
        void end_open() {}

        void end_closed()
        {
            line_to(_first_pt);
            if (_head_status == status::valid)
            {
                _f(_first_pt, vectors::unit(vectors::normal_cw(_first_tangent - _last_tangent)));
                _head_status = status::invalid;
            }
        }
        
    private:
        bool step(vector_t tangent)
        {
            if (_offset == 0)
            {
                if (_head_status == status::empty)
                {
                    _first_tangent = tangent;
                    _head_status = status::valid;
                }
                else
                    _f(_prev_pt, vectors::unit(vectors::normal_cw(tangent - _last_tangent)));
                // Next offset.
                while (_steps)
                {
                    T const next = _steps.next();
                    if (next > _length) [[likely]]
                    {
                        _offset = next - _length;
                        return true;
                    }
                }
                _offset = -1;
                return false;
            }
            if (_head_status == status::empty)
                _head_status = status::invalid;
            return _offset > 0;
        }

        void update(point_t pt, vector_t tangent)
        {
            _prev_pt = pt;
            _last_tangent = tangent;
        }

        template<class Actor>
        void do_act(T len, Actor&& act)
        {
            T const prev_length = _length;
            _length += len;
            if (_offset >= len)
            {
                _offset -= len;
                return;
            }
            act(_offset, len);
            while (_steps)
            {
                T const next = _steps.next();
                if (next >= _length)
                {
                    _offset = next - _length;
                    return;
                }
                if (next > prev_length) [[likely]]
                    act(next - prev_length, len);
            }
            _offset = -1;
        }

        T _offset, _length = 0;
        status _head_status = status::empty;
        Range _steps;
        F& _f;
        point_t _prev_pt, _first_pt;
        vector_t _last_tangent, _first_tangent;
    };

    template<class I, class E>
    struct fwd_range
    {
        I i;
        E const e;

        explicit operator bool() const
        {
            return i != e;
        }

        decltype(auto) next()
        {
            return *i++;
        }
    };

    template<class I, class E>
    fwd_range(I, E) -> fwd_range<I, E>;
}

namespace niji
{
    template<class Path, class Range, class F>
    void generate_tangents(Path const& path, Range const& steps, F&& f)
    {
        using std::begin;
        using std::end;
        using coord_t = path_coordinate_t<Path>;
        detail::fwd_range range{begin(steps), end(steps)};
        detail::tangents_sink<coord_t, decltype(range), F> sink(range, f);
        niji::iterate(path, sink);
    }
}

#endif