/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_ALGORITHM_HIT_TEST_HPP_INCLUDED
#define BOOST_NIJI_ALGORITHM_HIT_TEST_HPP_INCLUDED

#include <boost/niji/support/command.hpp>
#include <boost/niji/support/iterate.hpp>
#include <boost/niji/support/vector.hpp>
#include <boost/niji/support/point.hpp>
#include <boost/niji/support/bezier/basics.hpp>

namespace boost { namespace niji { namespace detail
{
    template<class T>
    struct hit_test_sink : stoppable_sink
    {
        point<T> o;
        bool result, pol;

        explicit hit_test_sink(point<T> const& o)
          : o(o), result(), pol()
        {}

        void operator()(move_to_t, point<T> const& pt)
        {
            _first = _prev = pt;
        }

        // non-zero:
        // pol += ((pt1.y > pt0.y) << 1) - 1;
        void operator()(line_to_t, point<T> pt1)
        {
            auto pt0 = _prev;
            _prev = pt1;
            
            if (pt1.y == o.y)
            {
                if (pt1.x == o.x || (pt0.y == o.y && (pt0.x < o.x) == (o.x < pt1.x)))
                  return on_boundary();
            }
            if ((pt0.y < o.y) != (pt1.y < o.y))
            {
                if (pt0.x < o.x)
                {
                    if (pt1.x <= o.x)
                    {
                        pol = !pol;
                        return;
                    }
                }
                else if (pt1.x > o.x)
                    return;
                T d = vectors::cross(pt0 - o, pt1 - o);
                if (!d)
                    return on_boundary();
                if ((d > 0) != (pt1.y > pt0.y))
                    pol = !pol;
            }
        }

        void operator()(quad_to_t, point<T> const& pt1, point<T> const& pt2)
        {
            auto pt0 = _prev;
            _prev = pt2;
            
            if (pt2 == o)
                return on_boundary();
            
            bool flag = pt2.y < o.y, flag_y = (pt0.y < o.y) != flag;
            if (flag_y || (pt1.y < o.y) != flag)
            {
                flag = pt2.x < o.x;
                bool flag2 = pt0.x < o.x;
                if (flag2 != flag || (pt1.x < o.x) != flag)
                {
                    T roots[2];
                    auto end = bezier::quad_solve(pt0.y, pt1.y, pt2.y, o.y, roots);
                    for (auto it = roots; it != end; ++it)
                    {
                        T x = bezier::quad_eval(pt0.x, pt1.x, pt2.x, *it);
                        if (x == o.x)
                          return on_boundary();
                        if (x < o.x)
                          pol = !pol;
                    }
                    pol ^= ((pt0.y == o.y) & flag2) != ((pt2.y == o.y) & flag);
                }
                else
                    pol ^= flag_y & flag;
            }
        }

        void operator()(cubic_to_t, point<T> pt1, point<T> pt2, point<T> pt3)
        {
            auto pt0 = _prev;
            _prev = pt3;
            
            if (pt3 == o)
                return on_boundary();
            
            bool flag = pt3.y < o.y, flag_y = (pt0.y < o.y) != flag;
            if (flag_y || (pt1.y < o.y) != flag || (pt2.y < o.y) != flag)
            {
                flag = pt3.x < o.x;
                bool flag2 = pt0.x < o.x;
                if (flag2 != flag || (pt1.x < o.x) != flag || (pt2.x < o.x) != flag)
                {
                    T roots[3];
                    auto end = bezier::cubic_solve(pt0.y, pt1.y, pt2.y, pt3.y, o.y, roots);
                    for (auto it = roots; it != end; ++it)
                    {
                        T x = bezier::cubic_eval(pt0.x, pt1.x, pt2.x, pt3.x, *it);
                        if (x == o.x)
                          return on_boundary();
                        if (x < o.x)
                          pol = !pol;
                    }
                    pol ^= ((pt0.y == o.y) & flag2) != ((pt3.y == o.y) & flag);
                }
                else
                    pol ^= flag_y & flag;
            }
        }
        
        void operator()(end_line_t)
        {
            pol = false;
        }

        void operator()(end_poly_t)
        {
            (*this)(line_to_t(), _first);
            result ^= pol;
            pol = false;
        }

        void on_boundary()
        {
            result = true;
            stop();
        }
        
    private:
        
        point<T> _prev, _first;
    };
}}}

namespace boost { namespace niji
{
    template<class Path, class Point>
    bool hit_test(Path const& path, Point const& pt)
    {
        using coord_t = path_coordinate_t<Path>;
        detail::hit_test_sink<coord_t> sink(pt);
        niji::iterate(path, sink);
        return sink.result;
    }
}}

#endif
