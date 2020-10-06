/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_TRANSFORM_HPP_INCLUDED
#define NIJI_VIEW_TRANSFORM_HPP_INCLUDED

#include <type_traits>
#include <niji/support/view.hpp>

namespace niji
{
    template<class F>
    struct transform_view : view<transform_view<F>>, F
    {
        template<class Path>
        using point_type = decltype(std::declval<F>()(std::declval<path_point_t<Path>>()));

        using F::F;

        transform_view() = default;
        transform_view(F&& f) : F(std::move(f)) {}
        transform_view(F const& f) : F(f) {}

        template<class Sink, class Point>
        struct adaptor
        {
            void move_to(Point const& pt)
            {
                _sink.move_to(_f(pt));
            }

            void line_to(Point const& pt)
            {
                _sink.line_to(_f(pt));
            }

            void quad_to(Point const& pt1, Point const& pt2)
            {
                _sink.quad_to(_f(pt1), _f(pt2));
            }

            void cubic_to(Point const& pt1, Point const& pt2, Point const& pt3)
            {
                _sink.cubic_to(_f(pt1), _f(pt2), _f(pt3));
            }

            void end_closed() { _sink.end_closed(); }
            void end_open() { _sink.end_open(); }

            Sink& _sink;
            F const& _f;
        };

        template<Path P, class Sink>
        void iterate(P const& path, Sink& sink) const
        {
            niji::iterate(path, adaptor<Sink, path_point_t<P>>{sink, *this});
        }

        template<BiPath P, class Sink>
        void reverse_iterate(P const& path, Sink& sink) const
        {
            niji::reverse_iterate(path, adaptor<Sink, path_point_t<P>>{sink, *this});
        }
    };
}

namespace niji::views
{
    template<class F>
    inline auto transform(F&& f)
    {
        return transform_view<std::decay_t<F>>{std::forward<F>(f)};
    }
}

#endif