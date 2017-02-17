/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2016-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_ALGORITHM_WINDING_HPP_INCLUDED
#define NIJI_ALGORITHM_WINDING_HPP_INCLUDED

#include <niji/render.hpp>
#include <niji/support/command.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/point.hpp>

namespace niji
{
    template<class T>
    struct winding_sink
    {
        bool is_ccw = false;

        void operator()(move_to_t, point<T> const& pt)
        {
            // _pts[0] & _pts[3] don't need to be set here
            _pts[1] = pt;
            _pts[2] = pt;
            _has_line = false;
            _has_turn = false;
            _first_is_min = true;
            is_ccw = false;
        }

        void operator()(line_to_t, point<T> const& pt)
        {
            next(pt);
        }

        void operator()(quad_to_t, point<T> const& pt1, point<T> const& pt2)
        {
            next(pt1);
            next(pt2);
        }

        void operator()(cubic_to_t, point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
        {
            next(pt1);
            next(pt2);
            next(pt3);
        }

        void operator()(end_open_t) {}

        void operator()(end_closed_t)
        {
            if (_has_turn)
            {
                auto p0 = _pts[_first_is_min ? 3 : 0];
                is_ccw = vectors::is_ccw(_pts[1] - p0, _pts[2] - _pts[1]);
            }
        }

    private:
        void next(point<T> const& pt)
        {
            if (pt.y < _pts[1].y)
            {
                _pts[0] = _pts[2 + _has_turn];
                _pts[1] = pt;
                _has_turn = false;
                _first_is_min = false;
            }
            else if (_has_turn)
                _pts[3] = pt;
            else if (pt != _pts[1])
            {
                _pts[2] = pt;
                _has_turn = _has_line;
            }
            _has_line = true;
        }

        bool _has_line;
        bool _has_turn;
        bool _first_is_min;
        point<T> _pts[4];
    };

    template<class Path>
    bool is_ccw(Path const& path)
    {
        using coord_t = path_coordinate_t<Path>;
        winding_sink<coord_t> sink;
        niji::render(path, sink);
        return sink.is_ccw;
    }
}

#endif