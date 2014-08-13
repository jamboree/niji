/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_SUPPORT_VIEW_HPP_INCLUDED
#define BOOST_NIJI_SUPPORT_VIEW_HPP_INCLUDED

#include <type_traits>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/utility/holder.hpp>
#include <boost/niji/support/utility/pack.hpp>

namespace boost { namespace niji
{
    template<class Path, class View>
    struct path_view
    {
        using point_type = typename std::decay_t<View>::
            template point_type<std::decay_t<Path>>;
        
        Path path;
        View view;
        
        template<class Sink>
        void iterate(Sink& sink) const
        {
            view.iterate(path, sink);
        }
        
        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            view.reverse_iterate(path, sink);
        }
    };

    template<class Derived>
    struct view
    {
        template<class Path, class... Ts>
        using point_type = typename path_point<Path>::type;

        template<class Path, class Sink, class... Ts>
        void reverse_iterate(Path const& path, Sink& sink, Ts const&... ts) const
        {
            using point_t = typename path_point<Path>::type;
            
            niji::path<point_t> tmp;
            typename niji::path<point_t>::cursor join(tmp, true);
            Derived::iterate(path, join, ts...);
            tmp.reverse_iterate(sink);
        }
    };
    
    template<class Derived, class Path>
    inline path_view<Path, Derived>
    operator|(Path&& path, view<Derived>&& d)
    {
        return {std::forward<Path>(path), static_cast<Derived&&>(d)};
    }
    
    template<class Derived, class Path>
    inline path_view<Path, holder_t<Derived const&>>
    operator|(Path&& path, view<Derived> const& d)
    {
        return {std::forward<Path>(path), static_cast<Derived const&>(d)};
    }
}}

#endif
