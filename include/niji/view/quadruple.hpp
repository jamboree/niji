/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_QUADRUPLE_HPP_INCLUDED
#define NIJI_VIEW_QUADRUPLE_HPP_INCLUDED

#include <niji/support/view.hpp>
#include <niji/view/reverse.hpp>
#include <niji/view/transform.hpp>

namespace niji::detail
{
    template<class Point, int quad>
    struct rot_quad
    {
        using coord_t = point_coordinate_t<Point>;
        
        auto operator()(Point const& pt) const
        {
            return call(get_x(pt), get_y(pt), std::integral_constant<int, quad>());
        }
        
        static auto call(coord_t x, coord_t y, std::integral_constant<int, 1>)
        {
            return point(-y, x);
        }
        
        static auto call(coord_t x, coord_t y, std::integral_constant<int, 2>)
        {
            return point(-x, -y);
        }
        
        static auto call(coord_t x, coord_t y, std::integral_constant<int, 3>)
        {
            return point(y, -x);
        }
    };
}

namespace niji
{
    struct quadruple_view : view<quadruple_view>
    {
        template<class Path>
        using point_type = point<path_coordinate_t<Path>>;

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

            niji::iterate(path, sink);
            niji::iterate(path | views::transform(detail::rot_quad<point_t, 1>()), sink);
            niji::iterate(path | views::transform(detail::rot_quad<point_t, 2>()), sink);
            niji::iterate(path | views::transform(detail::rot_quad<point_t, 3>()), sink);
        }
    };
}

namespace niji::views
{
    constexpr quadruple_view quadruple{};
}

#endif