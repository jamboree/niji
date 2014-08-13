/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_TRAITS_HPP_INCLUDED
#define BOOST_NIJI_TRAITS_HPP_INCLUDED

#include <boost/mpl/assert.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/utility/declval.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/geometry/core/tag.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/point_order.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/niji/path_fwd.hpp>
#include <boost/niji/support/utility/enable_if.hpp>

namespace boost { namespace niji { namespace detail
{
    BOOST_MPL_HAS_XXX_TRAIT_DEF(opposite)
    
    struct dummy_sink
    {
        template<class Tag, class Point>
        void operator()(Tag, Point const&) const;
        
        template<class Tag>
        void operator()(Tag) const;
    };
}}}
    
namespace boost { namespace niji
{
    // Geometry Traits
    //--------------------------------------------------------------------------
    template<class T>
    struct is_geometry
      : mpl::not_<is_same<typename geometry::traits::tag<T>::type, void> >
    {};
    
    template<class T>
    struct is_point
      : is_same<typename geometry::traits::tag<T>::type, geometry::point_tag>
    {};

    template<class T>
    struct is_box
      : is_same<typename geometry::traits::tag<T>::type, geometry::box_tag>
    {};
    
    template<geometry::order_selector>
    struct opposite_order;
    
    template<>
    struct opposite_order<geometry::counterclockwise>
      : integral_constant<geometry::order_selector, geometry::clockwise>
    {};

    template<>
    struct opposite_order<geometry::clockwise>
      : integral_constant<geometry::order_selector, geometry::counterclockwise>
    {};
    
    // Path Traits
    //--------------------------------------------------------------------------
    template<class Path, class Sink = detail::dummy_sink, class = void>
    struct is_iterable : mpl::false_ {};
    
    template<class Path, class Sink>
    struct is_iterable<Path, Sink, enable_if_valid_t<
        decltype(declval<Path>().iterate(declval<Sink&>()))>>
      : mpl::true_
    {};
    
    template<class Path, class Sink = detail::dummy_sink, class = void>
    struct is_reverse_iterable : mpl::false_ {};
    
    template<class Path, class Sink>
    struct is_reverse_iterable<Path, Sink, enable_if_valid_t<
        decltype(declval<Path>().reverse_iterate(declval<Sink&>()))>>
      : mpl::true_
    {};
    
    template<class Path, class = void>
    struct path_point
    {
        using type = typename Path::point_type;
    };
    
    template<class Path>
    using path_point_t = typename path_point<Path>::type;
    
    template<class Path>
    struct path_coordinate
    {
        using type =
            typename geometry::coordinate_type<path_point_t<Path>>::type;
    };
    
    template<class Path>
    using path_coordinate_t = typename path_coordinate<Path>::type;
    
    template<class Path, class Sink, class = void>
    struct path_iterate;
    
    template<class Path, class Sink>
    struct path_iterate<Path, Sink, enable_if_t<is_iterable<Path, Sink>>>
    {
        static void apply(Path const& path, Sink& sink)
        {
            path.iterate(sink);
        }
    };

    template<class Path, class Sink, class = void>
    struct path_reverse_iterate
    {
        static void apply(Path const& path, Sink& sink)
        {
            using point_type = typename path_point<Path>::type;
            
            niji::path<point_type> tmp;
            typename niji::path<point_type>::cursor join(tmp, true);
            path.iterate(join);
            tmp.reverse_iterate(sink);
        }
    };
    
    template<class Path, class Sink>
    struct path_reverse_iterate<Path, Sink,
        enable_if_t<is_reverse_iterable<Path, Sink>>>
    {
        static void apply(Path const& path, Sink& sink)
        {
            path.reverse_iterate(sink);
        }
    };

    template<class Tag, geometry::order_selector order>
    struct geometry_iterate
    {
        typedef
            geometry_iterate<Tag, opposite_order<order>::value>
        opposite;
        
        // You're seeing error here becasue geometry_iterate hasn't be defined
        // for either order of Tag.
        BOOST_MPL_ASSERT_NOT((detail::has_opposite<opposite>));
        
        template<class Geo, class Sink>
        static void apply(Geo const& geo, Sink& sink)
        {
            using point_t = typename geometry::point_type<Geo>::type;
            
            path<point_t> tmp;
            typename path<point_t>::joiner join(tmp);
            opposite::apply(geo, join);
            tmp.reverse_iterate(sink);
        }
    };
    
    template<class Tag, geometry::order_selector order>
    struct multi_geometry_iterate
    {
        template<class MultiGeometry, class Sink>
        static void apply(MultiGeometry const& multi_geometry, Sink& sink)
        {
            for (auto const& geometry : multi_geometry)
                geometry_iterate<Tag, order>::apply(geometry, sink);
        }
    };
    
    template<class Path>
    struct path_point<Path, enable_if_t<is_geometry<Path>>>
      : geometry::point_type<Path>
    {};
    
    template<class Path, class Sink>
    struct path_iterate<Path, Sink, enable_if_t<is_geometry<Path>>>
    {
        using tag_t = typename geometry::tag<Path>::type;

        static void apply(Path const& path, Sink& sink)
        {
            geometry_iterate<tag_t, geometry::counterclockwise>::apply(path, sink);
        } 
    };
    
    template<class Path, class Sink>
    struct path_reverse_iterate<Path, Sink, enable_if_t<is_geometry<Path>>>
    {
        using tag_t = typename geometry::tag<Path>::type;

        static void apply(Path const& path, Sink& sink)
        {
            geometry_iterate<tag_t, geometry::clockwise>::apply(path, sink);
        } 
    };

    template<class Path>
    struct path_cache
    {
        Path const& operator()(Path const& path)
        {
            return path;
        }
    };

    template<class Path>
    struct default_path_cache
    {
        using point_t = typename path_point<Path>::point_type;
        using cache_t = niji::path<point_t>;

        cache_t const& operator()(Path const& path)
        {
            return _cache = path;
        }
        
    private:
        
        cache_t _cache;
    };
    
    template<class Path, class Sink>
    void iterate(Path const& path, Sink&& sink)
    {
        path_iterate<Path, Sink>::apply(path, sink);
    }
    
    template<class Path, class Sink>
    void reverse_iterate(Path const& path, Sink&& sink)
    {
        path_reverse_iterate<Path, Sink>::apply(path, sink);
    }
}}

#endif
