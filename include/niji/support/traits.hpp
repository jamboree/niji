/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_TRAITS_HPP_INCLUDED
#define NIJI_SUPPORT_TRAITS_HPP_INCLUDED

#include <type_traits>
#include <boost/geometry/core/tag.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <niji/path_fwd.hpp>
#include <niji/detail/enable_if_valid.hpp>

namespace niji
{
    template<class T>
    using geometry_tag_t = typename boost::geometry::traits::tag<T>::type;

    template<class T>
    struct is_geometry
      : std::integral_constant<bool, !std::is_same<geometry_tag_t<T>, void>::value>
    {};
    
    template<class T>
    struct is_point
      : std::is_same<geometry_tag_t<T>, boost::geometry::point_tag>
    {};

    template<class T>
    struct is_box
      : std::is_same<geometry_tag_t<T>, boost::geometry::box_tag>
    {};

    template<class Path, class = void>
    struct path_point
    {
        using type = typename Path::point_type;
    };

    template<class Path>
    struct path_point<Path, std::enable_if_t<is_geometry<Path>::value>>
      : boost::geometry::point_type<Path>
    {};

    template<class Path>
    using path_point_t = typename path_point<Path>::type;
    
    template<class Path>
    struct path_coordinate
    {
        using type = typename
            boost::geometry::coordinate_type<path_point_t<Path>>::type;
    };
    
    template<class Path>
    using path_coordinate_t = typename path_coordinate<Path>::type;

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
        using cache_t = niji::path<path_point_t<Path>>;

        cache_t const& operator()(Path const& path)
        {
            return _cache = path;
        }
        
    private:
        
        cache_t _cache;
    };
}

#endif
