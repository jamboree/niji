/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

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
        using point_type = typename std::result_of<F(path_point_t<Path>)>::type;

        using F::F;

        template<class Sink>
        struct adaptor
        {
            template<class Tag, class... Points>
            void operator()(Tag tag, Points const&... pts) const
            {
                _sink(tag, _f(pts)...);
            }

            Sink& _sink;
            F const& _f;
        };

        template<class Path, class Sink>
        void render(Path const& path, Sink& sink) const
        {
            niji::render(path, adaptor<Sink>{sink, *this});
        }

        template<class Path, class Sink>
        void inverse_render(Path const& path, Sink& sink) const
        {
            niji::inverse_render(path, adaptor<Sink>{sink, *this});
        }
    };
}

namespace niji { namespace views
{
    template<class F>
    inline auto transform(F&& f)
    {
        return transform_view<std::decay_t<F>>{std::forward<F>(f)};
    }
}}

#endif
