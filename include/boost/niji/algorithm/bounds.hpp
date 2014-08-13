/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_ALGORITHM_BOUNDS_HPP_INCLUDED
#define BOOST_NIJI_ALGORITHM_BOUNDS_HPP_INCLUDED

#include <boost/assert.hpp>
#include <boost/integer.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/vector.hpp>
#include <boost/niji/support/point.hpp>
#include <boost/niji/support/box.hpp>
#include <boost/niji/support/bezier/basics.hpp>

namespace boost { namespace niji { namespace detail
{
    template<class T>
    struct bounds_calc
    {
        point<T> min, max;

        bounds_calc()
            : min(numeric::bounds<T>::highest(), numeric::bounds<T>::highest())
            , max(numeric::bounds<T>::lowest(), numeric::bounds<T>::lowest())
            , _moving(true)
        {}

        void operator()(move_to_t, point<T> const& pt)
        {
            _prev = pt;
            _moving = true;
        }

        void operator()(line_to_t, point<T> const& pt)
        {
            line_start();
            adjust(pt);
        }

        void operator()(quad_to_t, point<T> const& pt1, point<T> const& pt2)
        {
            line_start();
            adjust_quad(_prev, pt1, pt2);
        }

        void operator()(cubic_to_t, point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
        {
            // TODO
            BOOST_ASSERT_MSG(false, "feature not implemented yet");
        }
        
        void operator()(end_tag) {}
        
    private:

        void line_start()
        {
            if (_moving)
            {
                adjust_coord2<0>(_prev.x);
                adjust_coord2<1>(_prev.y);
                _moving = false;
            }
        }

        void adjust_quad(point<T> const& a, point<T> const& b, point<T> const& c)
        {
            if (b.x < min.x || b.x > max.x)
                adjust_quad_coord<0>(a, b, c);
            else
                adjust_coord<0>(c.x);
            if (b.y < min.y || b.y > max.y)
                adjust_quad_coord<1>(a, b, c);
            else
                adjust_coord<1>(c.y);
            _prev = c;
        }
        
        template<int n>
        void adjust_quad_coord(point<T> const& a, point<T> const& b, point<T> const& c)
        {
            using geometry::get;
            using geometry::set;

            T aa = get<n>(a), bb = get<n>(b), cc = get<n>(c);
            if (aa == bb || cc == bb)
                adjust_coord<n>(cc);
            
            if (aa > bb)
            {
                if (cc < bb && cc < get<n>(min))
                {
                    set<n>(min, cc);
                    return;
                }
            } 
            else if (cc > bb && cc > get<n>(max))
            {
                set<n>(max, cc);
                return;
            }
            adjust_coord<n>(find_quad_extrema(aa, bb, cc));
        }
        
        void adjust(point<T> const& pt)
        {
            adjust_coord<0>(pt.x);
            adjust_coord<1>(pt.y);
            _prev = pt;
        }
        
        template<int n>
        void adjust_coord(T val)
        {
            using geometry::get;
            using geometry::set;
            
            if (val < get<n>(min))
                set<n>(min, val);
            else if (val > get<n>(max))
                set<n>(max, val);
        }
        
        // for init that max < min
        template<int n>
        void adjust_coord2(T val)
        {
            using geometry::get;
            using geometry::set;
            
            if (val < get<n>(min))
                set<n>(min, val);
            if (val > get<n>(max))
                set<n>(max, val);
        }
        
        bool _moving;
        point<T> _prev;
    };
}}}

namespace boost { namespace niji
{
    template<class Path>
    auto bounds(Path const& path)
    {
        using coord_t = path_coordinate_t<Path>;
        detail::bounds_calc<coord_t> bounds;
        niji::iterate(path, bounds);
        return box<point_t>(bounds.min, bounds.max);
    }
}}

#endif
