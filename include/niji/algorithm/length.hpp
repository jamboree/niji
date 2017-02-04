/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_ALGORITHM_LENGTH_HPP_INCLUDED
#define NIJI_ALGORITHM_LENGTH_HPP_INCLUDED

#include <niji/render.hpp>
#include <niji/support/command.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/point.hpp>
#include <niji/support/bezier.hpp>

namespace niji { namespace detail
{
    template<class T>
    struct length_sink
    {
        T sum = 0;

        void operator()(move_to_t, point<T> const& pt)
        {
            _first = _prev = pt;
        }

        void operator()(line_to_t, point<T> const& pt)
        {
            sum += vectors::norm(pt - _prev);
            _prev = pt;
        }

        void operator()(quad_to_t, point<T> const& pt1, point<T> const& pt2)
        {
            sum += bezier::quad_length(_prev, pt1, pt2);
            _prev = pt2;
        }

        void operator()(cubic_to_t, point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
        {
            sum += bezier::cubic_length(_prev, pt1, pt2, pt3);
            _prev = pt3;
        }
        
        void operator()(end_open_t) {}

        void operator()(end_closed_t)
        {
            sum += vectors::norm(_first - _prev);
            _prev = _first;
        }
        
    private:
        
        point<T> _prev, _first;
    };
}}

namespace niji
{
    template<class Path>
    auto length(Path const& path)
    {
        using coord_t = path_coordinate_t<Path>;
        detail::length_sink<coord_t> accum;
        niji::render(path, accum);
        return accum.sum;
    }
}

#endif