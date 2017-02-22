/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_PAVE_HPP_INCLUDED
#define NIJI_VIEW_PAVE_HPP_INCLUDED

#include <type_traits>
#include <niji/support/view.hpp>
#include <niji/support/just.hpp>
#include <niji/view/rotate.hpp>
#include <niji/view/translate.hpp>
#include <niji/algorithm/generate_tangents.hpp>

namespace niji
{
    template<class T, class ItemPath>
    struct pave_view : view<pave_view<T, ItemPath>>
    {
        template<class Path>
        using point_type = point<T>;

        ItemPath item;
        T step;
        T offset;

        pave_view(ItemPath item, T step, T offset = {})
          : item(std::forward<ItemPath>(item)), step(step), offset(offset)
        {}

        template<class Path, class Sink>
        void render(Path const& path, Sink& sink) const
        {
            using coord_t = path_coordinate_t<Path>;
            using point_t = point<T>;
            using vector_t = vector<T>;
            generate_tangents(path, step, offset, [&](point_t const& pt, vector_t const& u)
            {
                niji::render(item | views::rotate<coord_t>(u.y, u.x) | views::translate<coord_t>(pt), sink);
            });
        }

        template<class Path, class Sink>
        void inverse_render(Path const& path, Sink& sink) const
        {
            using coord_t = path_coordinate_t<Path>;
            using point_t = point<T>;
            using vector_t = vector<T>;
            generate_tangents(path, step, offset, [&](point_t const& pt, vector_t const& u)
            {
                niji::inverse_render(item | views::rotate<coord_t>(u.y, u.x) | views::translate<coord_t>(pt), sink);
            });
        }
    };
}

namespace niji { namespace views
{
    template<class T, class ItemPath>
    inline pave_view<T, ItemPath> pave(ItemPath&& item, just_t<T> step, T offset = {})
    {
        return {std::forward<ItemPath>(item), step, offset};
    }
}}

#endif