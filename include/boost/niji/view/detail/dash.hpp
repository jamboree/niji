/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_VIEW_DETAIL_DASH_HPP_INCLUDED
#define BOOST_NIJI_VIEW_DETAIL_DASH_HPP_INCLUDED

#include <boost/optional/optional.hpp>
#include <boost/niji/path.hpp>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/point.hpp>
#include <boost/niji/support/vector.hpp>
#include <boost/niji/support/bezier/basics.hpp>

namespace boost { namespace niji { namespace detail
{
    template<class T, class Iterator>
    struct dasher
    {
        using point_t = point<T>;
        using vector_t = vector<T>;
        
        dasher(Iterator const& begin, Iterator const& end)
          : _begin(begin), _end(end), _it(begin)
          , _move_offset(), _line_offset(), _skip()
        {}
 
        void move_to(point_t const& pt)
        {
            _first_pt = _prev_pt = pt;
            _it = _begin;
            _move_offset = 0;
            _line_offset = 0;
        }

        template<class Sink>
        void quad(point_t const& pt1, point_t const& pt2, Sink& sink)
        {
            point_t pts[3] = {_prev_pt, pt1, pt2};
            T len(bezier::length(_prev_pt, pt1, pt2));
            quad_actor act{_path, pts};
            if (pre_act(act, len))
            {
                flush(sink);
                post_act(act, 0, len);
            }
            _prev_pt = pt2;
        }
        
        template<class Sink>
        void line(point_t const& pt, Sink& sink)
        {
            vector_t v(pt - _prev_pt);
            T len(vectors::norm(v));
            optional<point_t> join;
            T offset(_move_offset + _line_offset); // either is 0
            line_actor act{_path, pt, _prev_pt, v};
            if (pre_act(act, len))
            {
                flush(sink);
                if (act.join_pt)
                    _path.join(act.join_pt.get());
                post_act(act, offset, len);
            }
            _prev_pt = pt;
        }

        template<class Actor>
        bool pre_act(Actor& act, T& len)
        {
            if (_move_offset)
            {
                if (_move_offset >= len)
                {
                    _move_offset -= len;
                    return false;
                }
                act.post(_move_offset, len);
                _move_offset = 0;
            }
            else if (_line_offset)
            {
                if (_line_offset >= len)
                {
                    _line_offset -= len;
                    act.pad_end(true);
                    return false;
                }
                act.join(_line_offset, len);
                act.cut();
                _line_offset = 0;
            }
            else
                act.post();
            return true;
        }
        
        template<class Actor>
        void post_act(Actor& act, T sum, T len)
        {
            switch (_skip)
            {
            default:
                for ( ; ; )
                {
                    sum += *_it;
                    if (sum >= len)
                    {
                        _line_offset = sum - len;
                        act.pad_end(false);
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
                    act.cut();
            case true:
                    sum += *_it;
                    if (++_it == _end)
                        _it = _begin;
                    if (sum >= len)
                    {
                        _move_offset = sum - len;
                        _skip = false;
                        return;
                    }
                    _path.cut();
                    act.join(sum, len);
                }
            }
        }
        
        struct line_actor
        {
            path<point_t>& _path;
            point_t const& _pt;
            point_t const& _prev;
            vector_t const& _v;
            optional<point_t> join_pt;
            
            void pad_end(bool)
            {
                _path.join(_pt);
            }

            void join(T sum, T len)
            {
                _path.join(_prev + _v * sum / len);
            }

            void cut() {}
            
            void post(T sum, T len)
            {
                join_pt = _prev + _v * sum / len;
            }
            
            void post()
            {
                join_pt = _prev;
            }
        };
        
        struct quad_actor
        {
            path<point_t>& _path;
            point_t (&_pts)[3];
            point_t _chops[5];

            void pad_end(bool has_prev)
            {
                if (!has_prev)
                    _path.join(_pts[0]);
                _path.unsafe_quad_to(_pts[1], _pts[2]);
            }
            
            void join(T& sum, T& len)
            {
                bezier::chop_quad_at(_pts, _chops, sum / len);
                std::copy(_chops + 2, _chops + 5, _pts);
                len -= sum;
                sum = 0;
            }

            void cut()
            {
                _path.join_quad(_chops[0], _chops[1], _chops[2]);
            }

            void post(T& sum, T& len)
            {
                join(sum, len);
            }
            
            void post() {}
        };

        template<class Sink>
        void close(Sink& sink)
        {
            line(_first_pt, sink);
            flush(sink);
            reset();
        }
        
        template<class Sink>
        void cut(Sink& sink)
        {
            flush(sink);
            reset();
        }

        template<class Sink>
        void flush(Sink& sink)
        {
            _path.iterate(sink);
            _path.clear();
        }
        
        void reset()
        {
            _it = _begin;
            _move_offset = 0;
            _line_offset = 0;
            _skip = false;
        }

        Iterator const _begin, _end;
        Iterator _it;
        T _move_offset, _line_offset;
        point_t _prev_pt, _first_pt;
        path<point_t> _path;
        bool _skip;
    };
}}}

#endif
