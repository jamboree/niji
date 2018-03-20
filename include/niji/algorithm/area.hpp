/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2018 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_ALGORITHM_AREA_HPP_INCLUDED
#define NIJI_ALGORITHM_AREA_HPP_INCLUDED

#include <niji/render.hpp>
#include <niji/support/command.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/point.hpp>

// Reference:
// http://ich.deanmcnamee.com/graphics/2016/03/30/CurveArea.html

namespace niji { namespace detail
{
    template<class T>
    struct area_sink
    {
        T line_sum = 0;
        T quad_sum = 0;
        T cubic_sum = 0;

        void operator()(move_to_t, point<T> const& pt)
        {
            _first = _pt0 = pt;
        }

        void operator()(line_to_t, point<T> const& pt1)
        {
            line_sum += vectors::cross(_pt0, pt1);
            _pt0 = pt1;
        }

        void operator()(quad_to_t, point<T> const& pt1, point<T> const& pt2)
        {
            quad_sum +=
                2 * pt1.x * (pt2.y - _pt0.y) +
                _pt0.x * (2 * pt1.y + pt2.y) -
                pt2.x * (_pt0.y + 2 * pt1.y);

            _pt0 = pt2;
        }

        void operator()(cubic_to_t, point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
        {
            cubic_sum +=
                3 * pt1.x * (pt2.y + pt3.y - 2 * _pt0.y) +
                _pt0.x * (6 * pt1.y + 3 * pt2.y + pt3.y) -
                pt3.x * (_pt0.y + 3 * pt1.y + 6 * pt2.y) -
                3 * pt2.x * (_pt0.y + pt1.y - 2 * pt3.y);

            _pt0 = pt3;
        }
        
        void operator()(end_open_t) {}

        void operator()(end_closed_t)
        {
            line_sum += vectors::cross(_pt0, _first);
            _pt0 = _first;
        }
        
    private:
        point<T> _pt0, _first;
    };
}}

namespace niji
{
    template<class Path>
    auto area(Path const& path)
    {
        using coord_t = path_coordinate_t<Path>;
        detail::area_sink<coord_t> accum;
        niji::render(path, accum);
        return accum.line_sum / 2 + accum.quad_sum / 6 + accum.cubic_sum / 20;
    }
}

#endif