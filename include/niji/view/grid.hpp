/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_GRID_HPP_INCLUDED
#define NIJI_VIEW_GRID_HPP_INCLUDED

#include <niji/view/identity.hpp>
#include <niji/view/reverse.hpp>
#include <niji/view/translate.hpp>
#include <niji/support/view.hpp>
#include <niji/support/just.hpp>

namespace niji
{
    template<class T>
    struct grid_view : view<grid_view<T>>
    {
        template<class Path, class...>
        using point_type = point<path_coordinate_t<Path>>;

        std::size_t nx, ny;
        T dx, dy;

        grid_view(std::size_t nx, std::size_t ny, T dx, T dy)
            : nx(nx), ny(ny), dx(dx), dy(dy)
        {}

        template<Path P, class Sink>
        void iterate(P const& path, Sink& sink) const
        {
            iterate_impl(path, sink, nx, ny, dx, dy, views::identity);
        }

        template<BiPath P, class Sink>
        void reverse_iterate(P const& path, Sink& sink) const
        {
            iterate_impl(path, sink, nx, ny, dx, dy, views::reverse);
        }

    private:
        template<class Path, class Sink, class View>
        static void iterate_impl(Path const& path, Sink& sink, std::size_t nx, std::size_t ny, T dx, T dy, View view)
        {
            using coord_t = path_coordinate_t<Path>;

            views::translate<coord_t> translate(0, 0);
            auto transformed = path | view | translate;

            for (std::size_t i = 0; i != ny; ++i)
            {
                for (std::size_t j = 0; j != nx; ++j)
                {
                    transformed.iterate(sink);
                    translate.x += dx;
                }
                translate.x = 0;
                translate.y += dy;
            }
        }
    };
}

namespace niji::views
{
    template<class T>
    inline grid_view<T> grid(std::size_t nx, std::size_t ny, just_t<T> dx, just_t<T> dy)
    {
        return {nx, ny, dx, dy};
    }
}

#endif