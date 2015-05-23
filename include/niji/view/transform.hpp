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
    struct transform_view : view<transform_view<F>>
    {
        template<class Path>
        using point_type = typename std::result_of<F(path_point_t<Path>)>::type;

        F f;
        
        explicit transform_view(F f)
          : f(std::forward<F>(f))
        {}

        template<class Sink>
        struct adaptor
        {
            template<class Tag, class... Points>
            void operator()(Tag tag, Points const&... pts) const
            {
                _sink(tag, _f(pts)...);
            }

            Sink& _sink;
            std::add_lvalue_reference_t<F const> _f;
        };

        template<class Path, class Sink>
        void render(Path const& path, Sink& sink) const
        {
            niji::render(path, adaptor<Sink>{sink, f});
        }

        template<class Path, class Sink>
        void inverse_render(Path const& path, Sink& sink) const
        {
            niji::inverse_render(path, adaptor<Sink>{sink, f});
        }
    };
}

namespace niji { namespace views
{
    template<class F>
    inline auto transform(F&& f)
    {
        return transform_view<F>{std::forward<F>(f)};
    }
}}

#endif
