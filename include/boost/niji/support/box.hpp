/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_BOX_HPP_INCLUDED
#define BOOST_NIJI_BOX_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/core/coordinate_dimension.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/tags.hpp>
#include <boost/geometry/algorithms/make.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/vector.hpp>
#include <boost/niji/support/convert_geometry.hpp>
#include <boost/niji/support/utility/enable_if.hpp>

namespace boost { namespace niji
{
    template<class Point>
    struct box
    {
        using coordinate_type =
            typename geometry::traits::coordinate_type<Point>::type;
        
        using point_type = Point;
        
        using coord_t = coordinate_type;
        
        Point min_corner, max_corner;
        
        box() {}
        
        box(coord_t x1, coord_t y1, coord_t x2, coord_t y2)
          : min_corner(geometry::make<Point>(x1, y1))
          , max_corner(geometry::make<Point>(x2, y2))
        {}
        
        template<class Pt>
        box(Pt const& min_corner, Pt const& max_corner)
          : min_corner(convert_geometry<Point>(min_corner))
          , max_corner(convert_geometry<Point>(max_corner))
        {}

        template<class Pt>
        box(box<Pt> const& other)
          : min_corner(convert_geometry<Point>(other.min_corner))
          , max_corner(convert_geometry<Point>(other.max_corner))
        {}
                
        template<class Box, enable_if_t<is_box<Box>, bool> = true>
        box(Box const& other)
        {
            geometry::convert(other, *this);
        }

        void reset(coord_t x1, coord_t y1, coord_t x2, coord_t y2)
        {
            min_corner = geometry::make<Point>(x1, y1);
            max_corner = geometry::make<Point>(x2, y2);
        }
        
        template<class Pt>
        void reset(Pt const& min_corner, Pt const& max_corner)
        {
            min_corner = convert_geometry<Point>(min_corner);
            max_corner = convert_geometry<Point>(max_corner);
        }
        
        coordinate_type width() const
        {
            return length<0>();
        }

        coordinate_type height() const
        {
            return length<1>();
        }
                
        template<std::size_t n>
        coordinate_type length() const
        {
            using geometry::get;
            
            return get<n>(max_corner) - get<n>(min_corner);
        }
        
        bool empty() const
        {
            using geometry::get;

            return get<0>(max_corner) == get<0>(min_corner) ||
                   get<1>(max_corner) == get<1>(min_corner);
        }
        
        bool invalid()
        {
            using geometry::get;
            
            return get<0>(min_corner) > get<0>(max_corner) ||
                   get<1>(min_corner) > get<1>(max_corner);
        }
        
        void clear()
        {
            max_corner = min_corner = Point();
        }

        void translate(vector<coord_t> const& v)
        {
            translate(v.x, v.y);
        }

        void translate(coord_t x, coord_t y)
        {
            using geometry::get;
            using geometry::set;

            set<0>(min_corner, get<0>(min_corner) + x);
            set<1>(min_corner, get<1>(min_corner) + y);
            set<0>(max_corner, get<0>(max_corner) + x);
            set<1>(max_corner, get<1>(max_corner) + y);
        }

        void correct()
        {
            using geometry::get;
            using geometry::set;

            if (get<0>(max_corner) < get<0>(min_corner))
            {
                coordinate_type tmp(get<0>(min_corner));
                set<0>(min_corner, get<0>(max_corner));
                set<0>(max_corner, tmp);
            }
            if (get<1>(max_corner) < get<1>(min_corner))
            {
                coordinate_type tmp(get<1>(min_corner));
                set<1>(min_corner, get<1>(max_corner));
                set<1>(max_corner, tmp);
            }
        }

        template<class F>
        box transformed(F&& f) const
        {
            box ret(f(min_corner), f(max_corner));
            ret.correct();
            return ret;
        }

        void offset(vector<coord_t> const& v)
        {
            offset(v.x, v.y);
        }

        void offset(coord_t x, coord_t y)
        {
            using geometry::get;
            using geometry::set;

            set<0>(min_corner, get<0>(min_corner) - x);
            set<1>(min_corner, get<1>(min_corner) - y);
            set<0>(max_corner, get<0>(max_corner) + x);
            set<1>(max_corner, get<1>(max_corner) + y);
        }

        void expand(box const& other)
        {
            if (empty())
                *this = other;
            
            expand_coord<0>(other);
            expand_coord<1>(other);
        }
        
        template<std::size_t n>
        void expand_coord(box const& other)
        {
            using boost::geometry::get;
            using boost::geometry::set;
    
            if (get<n>(other.min_corner) < get<n>(min_corner))
                set<n>(min_corner, get<n>(other.min_corner));
            if (get<n>(other.max_corner) > get<n>(max_corner))
                set<n>(max_corner, get<n>(other.max_corner));
        }
        
        template<std::size_t n>
        Point const& corner() const
        {
            return *(&min_corner + n);
        }
        
        template<std::size_t n>
        Point& corner()
        {
            return *(&min_corner + n);
        }

        Point center() const
        {
            using geometry::get;
            using geometry::set;

            Point pt;
            set<0>(pt, (get<0>(min_corner) + get<0>(max_corner)) / 2);
            set<1>(pt, (get<1>(min_corner) + get<1>(max_corner)) / 2);
            return pt;
        }

        template<class Archive>
        void serialize(Archive & ar, unsigned version)
        {
            ar & min_corner & max_corner;
        }
    };
}}

namespace boost { namespace geometry { namespace traits
{
    template<class Point>
    struct tag<boost::niji::box<Point>>
    {
        using type = box_tag;
    };
    
    template<class Point>
    struct point_type<boost::niji::box<Point>>
    {
        using type = Point;
    };
    
    template<class Point, std::size_t Index, std::size_t Dimension>
    struct indexed_access<boost::niji::box<Point>, Index, Dimension>
    {
        using coordinate_type = typename geometry::coordinate_type<Point>::type;

        static inline coordinate_type get(boost::niji::box<Point> const& b)
        {
            return geometry::get<Dimension>(b.template corner<Index>());
        }
    
        static inline void set(boost::niji::box<Point>& b, coordinate_type value)
        {
            geometry::set<Dimension>(b.template corner<Index>(), value);
        }
    };
}}}

#endif
