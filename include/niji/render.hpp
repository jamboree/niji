/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_RENDER_HPP_INCLUDED
#define NIJI_RENDER_HPP_INCLUDED

#include <type_traits>
#include <niji/path_fwd.hpp>
#include <niji/geometry/render.hpp>
#include <niji/support/traits.hpp>
#include <niji/support/identifier.hpp>
#include <niji/detail/priority.hpp>
#include <niji/detail/enable_if_valid.hpp>

namespace niji { namespace detail
{
    template<class Path, class Sink>
    inline auto render_dispatch(priority<1>, Path const& path, Sink& sink) -> decltype(path.render(sink))
    {
        return path.render(sink);
    }

    template<class Path, class Sink>
    inline auto render_dispatch(priority<0>, Path const& path, Sink& sink) -> std::enable_if_t<is_geometry<Path>::value>
    {
        geometry_render<geometry_tag_t<Path>, boost::geometry::counterclockwise>::apply(path, sink);
    }

    template<class Path, class Sink>
    inline auto render(Path const& path, Sink& sink) -> decltype(render_dispatch(priority<1>{}, path, sink))
    {
        return render_dispatch(priority<1>{}, path, sink);
    }

    template<class Path, class Sink>
    inline auto inverse_render_dispatch(priority<2>, Path const& path, Sink& sink) -> decltype(path.inverse_render(sink))
    {
        return path.inverse_render(sink);
    }

    template<class Path, class Sink>
    inline auto inverse_render_dispatch(priority<1>, Path const& path, Sink& sink) -> std::enable_if_t<is_geometry<Path>::value>
    {
        geometry_render<geometry_tag_t<Path>, boost::geometry::clockwise>::apply(path, sink);
    }

    template<class Path, class Sink>
    inline auto inverse_render_dispatch(priority<0>, Path const& path, Sink& sink) -> decltype(render(path, sink))
    {
        niji::path<path_point_t<Path>> tmp(path);
        tmp.inverse_render(sink);
    }

    template<class Path, class Sink>
    inline auto inverse_render(Path const& path, Sink& sink) -> decltype(inverse_render_dispatch(priority<2>{}, path, sink))
    {
        return inverse_render_dispatch(priority<2>{}, path, sink);
    }

    template<class Path, class Sink>
    auto is_renderable_test(Path const& path, Sink& sink) -> enable_if_valid_t<decltype(render(path, sink)), std::true_type>;

    std::false_type is_renderable_test(...);

    struct render_fn
    {
        template<class Path, class Sink>
        auto operator()(Path const& path, Sink&& sink) const -> decltype(render(path, sink))
        {
            return render(path, sink);
        }
    };

    struct inverse_render_fn
    {
        template<class Path, class Sink>
        auto operator()(Path const& path, Sink&& sink) const -> decltype(inverse_render(path, sink))
        {
            return inverse_render(path, sink);
        }
    };
}}

namespace niji
{
    NIJI_IDENTIFIER(detail::render_fn, render)
    NIJI_IDENTIFIER(detail::inverse_render_fn, inverse_render)

    template<class Path, class Sink>
    struct is_renderable
      : decltype(detail::is_renderable_test(std::declval<Path>(), std::declval<std::add_lvalue_reference_t<Sink>>()))
    {};
}

#endif