/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2017-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_TRACE_HPP_INCLUDED
#define NIJI_VIEW_TRACE_HPP_INCLUDED

#include <type_traits>
#include <niji/support/view.hpp>
#include <niji/support/just.hpp>
#include <niji/view/rotate.hpp>
#include <niji/view/translate.hpp>
#include <niji/algorithm/generate_tangents.hpp>

namespace niji
{
    template<class T, class Footprint>
    struct trace_view : view<trace_view<T, Footprint>>
    {
        template<class Path>
        using point_type = point<T>;

        Footprint footprint;
        T step;
        T offset;

        trace_view(Footprint footprint, T step, T offset = {})
          : footprint(std::forward<Footprint>(footprint)), step(step), offset(offset)
        {}

        template<class Path, class Sink>
        void iterate(Path const& path, Sink& sink) const
        {
            using coord_t = path_coordinate_t<Path>;
            using point_t = point<T>;
            using vector_t = vector<T>;
            generate_tangents(path, step, offset, [&](point_t const& pt, vector_t const& u)
            {
                niji::iterate(footprint | views::rotate<coord_t>(u.y, u.x) | views::translate<coord_t>(pt), sink);
            });
        }

        template<class Path, class Sink>
        void reverse_iterate(Path const& path, Sink& sink) const
        {
            using coord_t = path_coordinate_t<Path>;
            using point_t = point<T>;
            using vector_t = vector<T>;
            generate_tangents(path, step, offset, [&](point_t const& pt, vector_t const& u)
            {
                niji::reverse_iterate(footprint | views::rotate<coord_t>(u.y, u.x) | views::translate<coord_t>(pt), sink);
            });
        }
    };
}

namespace niji::views
{
    template<class T, class Footprint>
    inline trace_view<T, Footprint> trace(Footprint&& footprint, just_t<T> step, T offset = {})
    {
        return {std::forward<Footprint>(footprint), step, offset};
    }
}

#endif