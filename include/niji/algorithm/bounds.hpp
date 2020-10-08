/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_ALGORITHM_BOUNDS_HPP_INCLUDED
#define NIJI_ALGORITHM_BOUNDS_HPP_INCLUDED

#include <limits>
#include <niji/core.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/point.hpp>
#include <niji/support/box.hpp>
#include <niji/support/bezier.hpp>

namespace niji::detail
{
    template<class T>
    struct bounds_sink
    {
        point<T> min, max;

        bounds_sink()
          : min(std::numeric_limits<T>::max(), std::numeric_limits<T>::max())
          , max(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest())
          , _moving(true)
        {}

        void move_to(point<T> const& pt)
        {
            _prev = pt;
            _moving = true;
        }

        void line_to(point<T> const& pt)
        {
            line_start();
            adjust(pt);
        }

        void quad_to(point<T> const& pt1, point<T> const& pt2)
        {
            line_start();
            adjust_quad(_prev, pt1, pt2);
        }

        void cubic_to(point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
        {
            line_start();
            adjust_cubic(_prev, pt1, pt2, pt3);
        }
        
        void end_open() {}

        void end_closed() {}
        
    private:
        void line_start()
        {
            if (_moving)
            {
                adjust_coord2(index<0>, _prev.x);
                adjust_coord2(index<1>, _prev.y);
                _moving = false;
            }
        }

        void adjust_quad(point<T> const& a, point<T> const& b, point<T> const& c)
        {
            if (b.x < min.x || b.x > max.x)
                adjust_quad_coord(index<0>, a, b, c);
            else
                adjust_coord(index<0>, c.x);
            if (b.y < min.y || b.y > max.y)
                adjust_quad_coord(index<1>, a, b, c);
            else
                adjust_coord(index<1>, c.y);
            _prev = c;
        }

        void adjust_cubic(point<T> const& a, point<T> const& b, point<T> const& c, point<T> const& d)
        {
            if (b.x < min.x || b.x > max.x || c.x < min.x || c.x > max.x)
                adjust_cubic_coord(index<0>, a, b, c, d);
            if (b.y < min.y || b.y > max.y || c.y < min.y || c.y > max.y)
                adjust_cubic_coord(index<1>, a, b, c, d);
            adjust(d);
        }
        
        template<int I>
        void adjust_quad_coord(index_constant<I> i, point<T> const& a, point<T> const& b, point<T> const& c)
        {
            T aa = a.coord(i), bb = b.coord(i), cc = c.coord(i);
            if (aa == bb || cc == bb)
            {
                adjust_coord(i, cc);
                return;
            }
            if (aa > bb)
            {
                if (cc < bb && cc < min.coord(i))
                {
                    min.coord(i) = cc;
                    return;
                }
            } 
            else if (cc > bb && cc > max.coord(i))
            {
                max.coord(i) = cc;
                return;
            }
            T t;
            auto end = find_quad_extrema(aa, bb, cc, &t);
            if (&t != end)
                adjust_coord(i, bezier::quad_eval(aa, bb, cc, t));
        }
        
        template<int I>
        void adjust_cubic_coord(index_constant<I> i, point<T> const& a, point<T> const& b, point<T> const& c, point<T> const& d)
        {
            T aa = a.coord(i), bb = b.coord(i), cc = c.coord(i), dd = d.coord(i);
            T lo = aa, hi = dd;
            if (hi < lo)
                std::swap(hi, lo);
            if (lo <= bb && bb <= hi && lo <= cc && cc <= hi)
                return;

            T extrama[2];
            auto end = find_cubic_extrema(aa, bb, cc, dd, extrama);
            for (auto it = extrama; it != end; ++it)
                adjust_coord(i, bezier::cubic_eval(aa, bb, cc, dd, *it));
        }
        
        void adjust(point<T> const& pt)
        {
            adjust_coord(index<0>, pt.x);
            adjust_coord(index<1>, pt.y);
            _prev = pt;
        }
        
        template<int I>
        void adjust_coord(index_constant<I> i, T val)
        {
            if (val < min.coord(i))
                min.coord(i) = val;
            else if (val > max.coord(i))
                max.coord(i) = val;
        }
        
        // For init that max < min.
        template<int I>
        void adjust_coord2(index_constant<I> i, T val)
        {
            if (val < min.coord(i))
                min.coord(i) = val;
            if (val > max.coord(i))
                max.coord(i) = val;
        }
        
        point<T> _prev;
        bool _moving;
    };
}

namespace niji
{
    template<Path P>
    auto bounds(P const& path)
    {
        using coord_t = path_coordinate_t<P>;
        using point_t = point<coord_t>;
        detail::bounds_sink<coord_t> bounds;
        niji::iterate(path, bounds);
        return box<coord_t>(bounds.min, bounds.max);
    }
}

#endif