/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_QUADRUPLE_HPP_INCLUDED
#define NIJI_VIEW_QUADRUPLE_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/algorithms/make.hpp>
#include <niji/support/view.hpp>
#include <niji/support/identifier.hpp>
#include <niji/view/inverse.hpp>
#include <niji/view/transform.hpp>

namespace niji { namespace detail
{
    template<class Point, int quad>
    struct rot_quad
    {
        using coord_t = typename boost::geometry::coordinate_type<Point>::type;
        
        Point operator()(Point const& pt) const
        {
            using boost::geometry::get;
            return call(get<0>(pt), get<1>(pt), std::integral_constant<int, quad>());
        }
        
        static Point call(coord_t x, coord_t y, std::integral_constant<int, 1>)
        {
            return boost::geometry::make<Point>(-y, x);
        }
        
        static Point call(coord_t x, coord_t y, std::integral_constant<int, 2>)
        {
            return boost::geometry::make<Point>(-x, -y);
        }
        
        static Point call(coord_t x, coord_t y, std::integral_constant<int, 3>)
        {
            return boost::geometry::make<Point>(y, -x);
        }
    };
}}

namespace niji
{
    struct quadruple_view : view<quadruple_view>
    {
        template<class Path, class Sink>
        static void render(Path const& path, Sink& sink)
        {
             render_impl(path, sink);
        }
        
        template<class Path, class Sink>
        static void inverse_render(Path const& path, Sink& sink)
        {
             render_impl(path | views::inverse, sink);
        }

    private:
        
        template<class Path, class Sink>
        static void render_impl(Path const& path, Sink& sink)
        {
            using point_t = path_point_t<Path>;

            path_cache<Path> cache;
            auto const& cached = cache(path);
            niji::render(cached, sink);
            niji::render(cached | views::transform(detail::rot_quad<point_t, 1>()), sink);
            niji::render(cached | views::transform(detail::rot_quad<point_t, 2>()), sink);
            niji::render(cached | views::transform(detail::rot_quad<point_t, 3>()), sink);
        }
    };
}

namespace niji { namespace views
{
    NIJI_IDENTIFIER(quadruple_view, quadruple);
}}

#endif