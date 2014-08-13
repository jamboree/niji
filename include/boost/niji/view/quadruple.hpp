/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_VIEW_QUADRUPLE_HPP_INCLUDED
#define BOOST_NIJI_VIEW_QUADRUPLE_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/algorithms/make.hpp>
#include <boost/niji/view/reverse.hpp>
#include <boost/niji/view/transform.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/view.hpp>

namespace boost { namespace niji { namespace detail
{
    template<class Point, int quad>
    struct rot_quad
    {
        using coord_t = typename geometry::coordinate_type<Point>::type;
        
        Point operator()(Point const& pt) const
        {
            using geometry::get;
            return call(get<0>(pt), get<1>(pt), mpl::int_<quad>());
        }
        
        static Point call(coord_t x, coord_t y, mpl::int_<1>)
        {
            return geometry::make<Point>(-y, x);
        }
        
        static Point call(coord_t x, coord_t y, mpl::int_<2>)
        {
            return geometry::make<Point>(-x, -y);
        }
        
        static Point call(coord_t x, coord_t y, mpl::int_<3>)
        {
            return geometry::make<Point>(y, -x);
        }
    };
}}}

namespace boost { namespace niji
{
    struct quadruple_view : view<quadruple_view>
    {
        template<class Path, class Sink>
        static void iterate(Path const& path, Sink& sink)
        {
             iterate_impl(path, sink);
        }
        
        template<class Path, class Sink>
        static void reverse_iterate(Path const& path, Sink& sink)
        {
             iterate_impl(path | views::reverse, sink);
        }

    private:
        
        template<class Path, class Sink>
        static void iterate_impl(Path const& path, Sink& sink)
        {
            using point_t = path_point_t<Path>;

            path_cache<Path> cache;
            auto const& cached = cache(path);
            niji::iterate(cached, sink);
            niji::iterate(cached | views::transform(detail::rot_quad<point_t, 1>()), sink);
            niji::iterate(cached | views::transform(detail::rot_quad<point_t, 2>()), sink);
            niji::iterate(cached | views::transform(detail::rot_quad<point_t, 3>()), sink);
        }
    };
}}

namespace boost { namespace niji { namespace views
{
    constexpr quadruple_view quadruple {};
}}}

#endif
