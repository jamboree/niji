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
#include <niji/support/range.hpp>
#include <niji/view/identity.hpp>
#include <niji/view/reverse.hpp>
#include <niji/view/rotate.hpp>
#include <niji/view/translate.hpp>
#include <niji/algorithm/generate_tangents.hpp>

namespace niji
{
    template<class T, class Foots, class Steps>
    struct trace_view : view<trace_view<T, Foots, Steps>>
    {
        template<class Path>
        using point_type = point<T>;

        Foots foots;
        Steps steps;

        trace_view(Foots foots, Steps steps)
          : foots(std::forward<Foots>(foots)), steps(std::forward<Steps>(steps))
        {}

        template<Path P, class Sink>
        void iterate(P const& path, Sink& sink) const
        {
            iterate_impl(path, sink, views::identity);
        }

        template<Path P, class Sink>
        void reverse_iterate(P const& path, Sink& sink) const requires BiPath<ranges::range_value_t<Foots>>
        {
            iterate_impl(path, sink, views::reverse);
        }

    private:
        template<class Path, class Sink, class View>
        void iterate_impl(Path const& path, Sink& sink, View view) const
        {
            using coord_t = path_coordinate_t<Path>;
            using point_t = point<T>;
            using vector_t = vector<T>;
            std::pair range{ranges::begin(foots), ranges::end(foots)};
            generate_tangents(path, steps, [&](point_t const& pt, vector_t const& u)
            {
                if (range.first != range.second)
                {
                    niji::iterate(*range.first | view | views::rotate<coord_t>(u.y, u.x) | views::translate<coord_t>(pt), sink);
                    ++range.first;
                }
            });
        }
    };
}

namespace niji::views
{
    template<class T, class Foots, class Steps>
    inline trace_view<T, Foots, Steps> trace(Foots&& foots, Steps&& steps)
    {
        return {std::forward<Foots>(foots), std::forward<Steps>(steps)};
    }
}

#endif