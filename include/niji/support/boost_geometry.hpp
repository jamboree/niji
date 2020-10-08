/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_BOOST_GEOMETRY_HPP_INCLUDED
#define NIJI_SUPPORT_BOOST_GEOMETRY_HPP_INCLUDED

#include <type_traits>
#include <niji/core.hpp>
#include <niji/support/box.hpp>
#include <niji/support/range.hpp>
#include <boost/geometry/core/tag.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/point_order.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/geometry/algorithms/make.hpp>
#include <boost/range/adaptor/reversed.hpp>

namespace boost::geometry::traits
{
    template<class T>
    struct tag<niji::point<T>>
    {
        using type = point_tag;
    };

    template<class T>
    struct coordinate_type<niji::point<T>>
    {
        using type = T;
    };

    template<class T>
    struct coordinate_system<niji::point<T>>
    {
        using type = cs::cartesian;
    };

    template<class T>
    struct dimension<niji::point<T>> : boost::mpl::int_<2> {};

    template<class T, std::size_t Dimension>
    struct access<niji::point<T>, Dimension>
    {
        using point_type = niji::point<T>;

        static inline T get(point_type const& p)
        {
            return p.coord(niji::index<Dimension>);
        }

        static inline void set(point_type& p, T value)
        {
            p.coord(niji::index<Dimension>) = value;
        }
    };

    template<class T>
    struct tag<niji::box<T>>
    {
        using type = box_tag;
    };

    template<class Point>
    struct point_type<niji::box<Point>>
    {
        using type = Point;
    };

    template<class T, std::size_t Index, std::size_t Dimension>
    struct indexed_access<niji::box<T>, Index, Dimension>
    {
        using coordinate_type = T;

        static inline coordinate_type get(niji::box<T> const& b)
        {
            return b.corner(niji::index<Index>).coord(niji::index<Dimension>);
        }

        static inline void set(niji::box<T>& b, coordinate_type value)
        {
            b.corner(niji::index<Index>).coord(niji::index<Dimension>) = value;
        }
    };
}

namespace niji::detail
{
    template<class T>
    using geometry_tag_t = typename boost::geometry::traits::tag<T>::type;

    template<class T, class Tag>
    concept BoostGeometry = std::is_same_v<geometry_tag_t<T>, Tag>;

    template<class T>
    concept BoostMulitiGeometry = std::is_base_of_v<boost::geometry::multi_tag, geometry_tag_t<T>>;

    template<class Segment, class Sink>
    void iterate_segment(Segment const& segment, Sink& sink)
    {
        using boost::geometry::get;
        sink.move_to(get<0>(segment));
        sink.line_to(get<1>(segment));
        sink.end_open();
    }

    template<class Box, class Sink>
    void iterate_box(Box const& box, Sink& sink, bool ccw)
    {
        using boost::geometry::get;
        using boost::geometry::make;
        using coord_t = typename boost::geometry::coordinate_type<Box>::type;
        using point_t = typename boost::geometry::point_type<Box>::type;

        coord_t const x[2] = {get<boost::geometry::min_corner, 0>(box), get<boost::geometry::max_corner, 0>(box)};
        coord_t const y[2] = {get<boost::geometry::min_corner, 1>(box), get<boost::geometry::max_corner, 1>(box)};

        sink.move_to(make<point_t>(x[0], y[0]));
        sink.line_to(make<point_t>(x[ccw], y[!ccw]));
        sink.line_to(make<point_t>(x[1], y[1]));
        sink.line_to(make<point_t>(x[!ccw], y[ccw]));
        sink.end_closed();
    }

    template<class Points, class Sink>
    bool iterate_points(Points const& points, Sink& sink)
    {
        using std::begin;
        using std::end;

        auto it = begin(points);
        auto e = end(points);

        if (it == e)
            return false;

        sink.move_to(*it);
        while (++it != e)
            sink.line_to(*it);
        return true;
    }

    template<class Ring, class Sink>
    bool iterate_ring(Ring const& ring, Sink& sink)
    {
        if (iterate_points(ring, sink))
        {
            sink.end_closed();
            return true;
        }
        return false;
    }

    template<boost::geometry::order_selector Order, class T>
    using adjust_order_t = std::conditional_t
        <
        boost::geometry::point_order<T>::value == Order
        , T const&, boost::reversed_range<T const>
        >;

    template<boost::geometry::order_selector Order, class T>
    inline adjust_order_t<Order, T> adjust_order(T const& t)
    {
        return adjust_order_t<Order, T>(t);
    }

    template<boost::geometry::order_selector Order, class T, class Sink>
    void iterate_polygon(T const& polygon, Sink& sink)
    {
        if (iterate_ring(adjust_order<Order>(boost::geometry::exterior_ring(polygon)), sink))
        {
            for (auto const& ring : boost::geometry::interior_rings(polygon))
                iterate_ring(adjust_order<Order>(ring), sink);
        }
    }
}

namespace niji
{
    template<class T> requires detail::BoostGeometry<T, boost::geometry::point_tag>
    struct impl_point<T>
    {
        using coordinate_type = typename boost::geometry::coordinate_type<T>::type;

        static coordinate_type get_x(T const& self) { return boost::geometry::get<0>(self); }
        static coordinate_type get_y(T const& self) { return boost::geometry::get<1>(self); }
    };

    template<class T> requires detail::BoostGeometry<T, boost::geometry::segment_tag>
    struct impl_path<T>
    {
        using point_type = typename boost::geometry::point_type<T>::type;

        template<class Sink>
        static void iterate(T const& self, Sink& sink)
        {
            detail::iterate_segment(self, sink);
        }
    };

    template<class T> requires detail::BoostGeometry<T, boost::geometry::linestring_tag>
    struct impl_path<T>
    {
        using point_type = typename boost::geometry::point_type<T>::type;

        template<class Sink>
        static void iterate(T const& self, Sink& sink)
        {
            if (detail::iterate_points(self, sink))
                sink.end_open();
        }
    };
    
    template<class T> requires detail::BoostGeometry<T, boost::geometry::box_tag> && (!Iteratable<T>)
    struct impl_path<T>
    {
        using point_type = typename boost::geometry::point_type<T>::type;

        template<class Sink>
        static void iterate(T const& self, Sink& sink)
        {
            detail::iterate_box(self, sink, true);
        }
    };

    template<class T> requires detail::BoostGeometry<T, boost::geometry::box_tag> && (!ReverseIteratable<T>)
    struct impl_bi_path<T>
    {
        template<class Sink>
        static void reverse_iterate(T const& self, Sink& sink)
        {
            detail::iterate_box(self, sink, false);
        }
    };
    
    template<class T> requires detail::BoostGeometry<T, boost::geometry::ring_tag>
    struct impl_path<T>
    {
        using point_type = typename boost::geometry::point_type<T>::type;

        template<class Sink>
        static void iterate(T const& self, Sink& sink)
        {
            detail::iterate_ring(detail::adjust_order<boost::geometry::counterclockwise>(self), sink);
        }
    };

    template<class T> requires detail::BoostGeometry<T, boost::geometry::ring_tag>
    struct impl_bi_path<T>
    {
        template<class Sink>
        static void reverse_iterate(T const& self, Sink& sink)
        {
            detail::iterate_ring(detail::adjust_order<boost::geometry::clockwise>(self), sink);
        }
    };

    template<class T> requires detail::BoostGeometry<T, boost::geometry::polygon_tag>
    struct impl_path<T>
    {
        using point_type = typename boost::geometry::point_type<T>::type;

        template<class Sink>
        static void iterate(T const& self, Sink& sink)
        {
            detail::iterate_polygon<boost::geometry::counterclockwise>(self, sink);
        }
    };

    template<class T> requires detail::BoostGeometry<T, boost::geometry::polygon_tag>
    struct impl_bi_path<T>
    {
        template<class Sink>
        static void reverse_iterate(T const& self, Sink& sink)
        {
            detail::iterate_polygon<boost::geometry::clockwise>(self, sink);
        }
    };
    
    template<detail::BoostMulitiGeometry T> requires Path<ranges::range_value_t<T>>
    struct impl_path<T>
    {
        using point_type = typename boost::geometry::point_type<T>::type;

        template<class Sink>
        static void iterate(T const& self, Sink& sink)
        {
            for (auto const& part : self)
                niji::iterate(part, sink);
        }
    };

    template<detail::BoostMulitiGeometry T> requires BiPath<ranges::range_value_t<T>>
    struct impl_bi_path<T>
    {
        template<class Sink>
        static void reverse_iterate(T const& self, Sink& sink)
        {
            for (auto const& part : self)
                niji::reverse_iterate(part, sink);
        }
    };
}

#endif