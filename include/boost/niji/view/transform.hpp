/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_VIEW_TRANSFORM_HPP_INCLUDED
#define BOOST_NIJI_VIEW_TRANSFORM_HPP_INCLUDED

#include <type_traits>
#include <boost/niji/support/view.hpp>
#include <boost/niji/support/utility/holder.hpp>

namespace boost { namespace niji
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
            holder_t<std::add_lvalue_reference_t<F const>> _f;
        };

        template<class Path, class Sink>
        void iterate(Path const& path, Sink& sink) const
        {
            niji::iterate(path, adaptor<Sink>{sink, f});
        }

        template<class Path, class Sink>
        void reverse_iterate(Path const& path, Sink& sink) const
        {
            niji::reverse_iterate(path, adaptor<Sink>{sink, f});
        }
    };
}}

namespace boost { namespace niji { namespace views
{
    template<class F>
    inline auto transform(F&& f)
    {
        return transform_view<holder_t<F>>{std::forward<F>(f)};
    }
}}}

#endif
