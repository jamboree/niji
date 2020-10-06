/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_DETAIL_DASH_HPP_INCLUDED
#define NIJI_VIEW_DETAIL_DASH_HPP_INCLUDED

#include <cmath>
#include <niji/path.hpp>
#include <niji/support/point.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/bezier.hpp>

namespace niji::detail
{
    template<class T, class U, class Iterator>
    struct dasher
    {
        using point_t = point<T>;
        using vector_t = vector<T>;
        
        dasher(Iterator const& begin, Iterator const& end, T offset, U weight)
          : _begin(begin), _end(end), _it(begin)
          , _offset(), _weight(weight), _skip(), _gap()
        {
            using std::fmod;
            
            assert(begin != end);

            if (offset > 0)
            {
                for (T sum = 0; ; )
                {
                    sum += _weight * (*_it);
                    if (sum >= offset)
                    {
                        _offset = sum - offset;
                        if (++_it == end)
                            _it = begin;
                        else
                            _skip = !_gap;
                        break;
                    }
                    if (++_it == end)
                    {
                        _it = begin;
                        _gap = false;
                        offset = fmod(offset, sum);
                        sum = 0;
                        continue;
                    }
                    _gap = !_gap;
                }
            }
        }

        void move_to(point_t const& pt)
        {
            _first_pt = _prev_pt = pt;
        }

        template<class Sink>
        void line_to(point_t const& pt, Sink& sink)
        {
            vector_t v(pt - _prev_pt);
            T len(vectors::norm(v));
            line_actor act{_path, {_prev_pt, pt}};
            if (pre_act(act, len))
            {
                flush(sink);
                post_act(act, len);
            }
            _prev_pt = pt;
        }

        template<class Sink>
        void quad_to(point_t const& pt1, point_t const& pt2, Sink& sink)
        {
            T len(bezier::quad_length(_prev_pt, pt1, pt2));
            quad_actor act{_path, {_prev_pt, pt1, pt2}};
            if (pre_act(act, len))
            {
                flush(sink);
                post_act(act, len);
            }
            _prev_pt = pt2;
        }

        template<class Sink>
        void cubic_to(point_t const& pt1, point_t const& pt2, point_t const& pt3, Sink& sink)
        {
            T len(bezier::cubic_length(_prev_pt, pt1, pt2, pt3));
            cubic_actor act{_path, {_prev_pt, pt1, pt2, pt3}};
            if (pre_act(act, len))
            {
                flush(sink);
                post_act(act, len);
            }
            _prev_pt = pt3;
        }
        
        template<class Actor>
        bool pre_act(Actor& act, T& len)
        {
            if (_offset)
            {
                if (_offset >= len)
                {
                    _offset -= len;
                    if (!_gap)
                        act.join_end(true);
                    return false;
                }
                act.join(_offset, len);
                len -= _offset, _offset = 0;
                if (!_gap)
                    act.cut();
            }
            return true;
        }
        
        template<class Actor>
        void post_act(Actor& act, T len)
        {
            T sum = 0;
            switch (_skip)
            {
            default:
                for (;;)
                {
                    sum += _weight * (*_it);
                    if (sum >= len)
                    {
                        _offset = sum - len;
                        _gap = false;
                        act.join_end(false);
                        if (++_it == _end)
                            _it = _begin;
                        else
                            _skip = true;
                        return;
                    }
                    if (++_it == _end)
                    {
                        _it = _begin;
                        continue;
                    }
                    act.join(sum, len);
                    len -= sum, sum = 0;
                    act.cut();
            case true:
                    sum += _weight * (*_it);
                    if (++_it == _end)
                        _it = _begin;
                    if (sum >= len)
                    {
                        _offset = sum - len;
                        _gap = true;
                        _skip = false;
                        return;
                    }
                    _path.cut();
                    act.join(sum, len);
                    len -= sum, sum = 0;
                }
            }
        }
        
        struct line_actor
        {
            path<point_t>& _path;
            point_t _pts[2];
            point_t _chops[3];

            void join_end(bool has_prev)
            {
                if (!has_prev)
                    _path.join(_pts[0]);
                _path.join(_pts[1]);
            }

            void join(T sum, T len)
            {
                _chops[0] = _pts[0];
                _chops[1] = points::interpolate(_pts[0], _pts[1], sum / len);
                _chops[2] = _pts[1];
                std::copy(_chops + 1, _chops + 3, _pts);
            }

            void cut()
            {
                _path.join(_chops[0]);
                _path.join(_chops[1]);
            }
        };

        struct quad_actor
        {
            path<point_t>& _path;
            point_t _pts[3];
            point_t _chops[5];

            void join_end(bool has_prev)
            {
                if (!has_prev)
                    _path.join(_pts[0]);
                _path.unsafe_quad_to(_pts[1], _pts[2]);
            }
            
            void join(T sum, T len)
            {
                bezier::curve_bisect(NIJI_MAX_QUAD_SUBDIVIDE, sum, len, [this](T t)
                {
                    bezier::chop_quad_at(_pts, _chops, t);
                    return bezier::quad_length(_chops[0], _chops[1], _chops[2]);
                });
                std::copy(_chops + 2, _chops + 5, _pts);
            }

            void cut()
            {
                _path.join_quad(_chops[0], _chops[1], _chops[2]);
            }
        };
        
        struct cubic_actor
        {
            path<point_t>& _path;
            point_t _pts[4];
            point_t _chops[7];

            void join_end(bool has_prev)
            {
                if (!has_prev)
                    _path.join(_pts[0]);
                _path.unsafe_cubic_to(_pts[1], _pts[2], _pts[3]);
            }
            
            void join(T sum, T len)
            {
                bezier::curve_bisect(NIJI_MAX_CUBIC_SUBDIVIDE, sum, len, [this](T t)
                {
                    bezier::chop_cubic_at(_pts, _chops, t);
                    return bezier::cubic_length(_chops[0], _chops[1], _chops[2], _chops[3]);
                });
                std::copy(_chops + 3, _chops + 7, _pts);
            }

            void cut()
            {
                _path.join_cubic(_chops[0], _chops[1], _chops[2], _chops[3]);
            }
        };

        template<class Sink>
        void close(Sink& sink)
        {
            line_to(_first_pt, sink);
            flush(sink);
        }
        
        template<class Sink>
        void cut(Sink& sink)
        {
            flush(sink);
        }

        template<class Sink>
        void flush(Sink& sink)
        {
            _path.iterate(sink);
            _path.clear();
        }

        path<point_t> _path;
        Iterator const _begin, _end;
        Iterator _it;
        point_t _prev_pt, _first_pt;
        T _offset;
        U _weight;
        bool _skip, _gap;
    };
}

#endif