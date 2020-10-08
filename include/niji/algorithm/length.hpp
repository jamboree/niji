/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_ALGORITHM_LENGTH_HPP_INCLUDED
#define NIJI_ALGORITHM_LENGTH_HPP_INCLUDED

#include <niji/core.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/point.hpp>
#include <niji/support/bezier.hpp>

namespace niji::detail
{
    template<class T>
    struct length_sink
    {
        T sum = 0;

        void move_to(point<T> const& pt)
        {
            _first = _prev = pt;
        }

        void line_to(point<T> const& pt)
        {
            sum += vectors::norm(pt - _prev);
            _prev = pt;
        }

        void quad_to(point<T> const& pt1, point<T> const& pt2)
        {
            sum += bezier::quad_length(_prev, pt1, pt2);
            _prev = pt2;
        }

        void cubic_to(point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
        {
            sum += bezier::cubic_length(_prev, pt1, pt2, pt3);
            _prev = pt3;
        }
        
        void end_open() {}

        void end_closed()
        {
            sum += vectors::norm(_first - _prev);
            _prev = _first;
        }
        
    private:
        point<T> _prev, _first;
    };
}

namespace niji
{
    template<Path P>
    auto length(P const& path)
    {
        using coord_t = path_coordinate_t<P>;
        detail::length_sink<coord_t> accum;
        niji::iterate(path, accum);
        return accum.sum;
    }
}

#endif