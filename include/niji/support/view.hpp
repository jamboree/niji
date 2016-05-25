/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_VIEW_HPP_INCLUDED
#define NIJI_SUPPORT_VIEW_HPP_INCLUDED

#include <type_traits>
#include <niji/render.hpp>

namespace niji
{
    template<class Path, class View>
    struct path_adaptor
    {
        using point_type = typename std::decay_t<View>::
            template point_type<std::decay_t<Path>>;
        
        Path path;
        View view;
        
        template<class Sink>
        void render(Sink& sink) const
        {
            view.render(path, sink);
        }
        
        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            view.inverse_render(path, sink);
        }
    };

    template<class Derived>
    struct view
    {
        template<class Path>
        using point_type = typename path_point<Path>::type;

        template<class Path, class Sink>
        void inverse_render(Path const& path, Sink& sink) const
        {
            using point_t = typename Derived::template point_type<Path>;
            
            niji::path<point_t> tmp;
            typename niji::path<point_t>::cursor join(tmp, true);
            Derived::render(path, join);
            tmp.inverse_render(sink);
        }
    };

    template<class LHS, class RHS>
    struct composite_view : view<composite_view<LHS, RHS>>
    {
        LHS lhs;
        RHS rhs;

        composite_view(LHS&& lhs, RHS&& rhs)
          : lhs(std::forward<LHS>(lhs)), rhs(std::forward<RHS>(rhs))
        {}

        template<class Path, class Sink>
        void render(Path const& path, Sink& sink) const
        {
            niji::render(path | lhs | rhs, sink);
        }

        template<class Path, class Sink>
        void inverse_render(Path const& path, Sink& sink) const
        {
            niji::inverse_render(path | lhs | rhs, sink);
        }
    };

    namespace detail
    {
        template<class Derived>
        std::true_type is_view_test(view<Derived> const&);

        std::false_type is_view_test(...);
    }

    template<class View>
    struct is_view
      : decltype(detail::is_view_test(std::declval<View>()))
    {};
    
    template<class Derived, class Path>
    inline path_adaptor<Path, Derived>
    operator|(Path&& path, view<Derived>&& d)
    {
        return {std::forward<Path>(path), static_cast<Derived&&>(d)};
    }
    
    template<class Derived, class Path>
    inline path_adaptor<Path, Derived const&>
    operator|(Path&& path, view<Derived> const& d)
    {
        return {std::forward<Path>(path), static_cast<Derived const&>(d)};
    }

    template<class LHS, class RHS>
    inline composite_view<LHS, RHS>
    operator||(view<LHS>&& lhs, view<RHS>&& rhs)
    {
        return {static_cast<LHS&&>(lhs), static_cast<RHS&&>(rhs)};
    }

    template<class LHS, class RHS>
    inline composite_view<LHS const&, RHS const&>
    operator||(view<LHS> const& lhs, view<RHS> const& rhs)
    {
        return {static_cast<LHS const&>(lhs), static_cast<RHS const&>(rhs)};
    }
}

#endif