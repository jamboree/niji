/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2016-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_ALGORITHM_WINDING_HPP_INCLUDED
#define NIJI_ALGORITHM_WINDING_HPP_INCLUDED

#include <niji/core.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/point.hpp>

namespace niji
{
    template<class T>
    struct winding_sink
    {
        bool is_ccw = false;

        void move_to(point<T> const& pt)
        {
            // _pts[0] & _pts[3] don't need to be set here
            _pts[1] = pt;
            _pts[2] = pt;
            _has_line = false;
            _has_turn = false;
            _first_is_min = true;
            is_ccw = false;
        }

        void line_to(point<T> const& pt)
        {
            next(pt);
        }

        void quad_to(point<T> const& pt1, point<T> const& pt2)
        {
            next(pt1);
            next(pt2);
        }

        void cubic_to(point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
        {
            next(pt1);
            next(pt2);
            next(pt3);
        }

        void end_open() {}

        void end_closed()
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

    template<Path P>
    bool is_ccw(P const& path)
    {
        using coord_t = path_coordinate_t<P>;
        winding_sink<coord_t> sink;
        niji::iterate(path, sink);
        return sink.is_ccw;
    }
}

#endif