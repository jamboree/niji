/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_VIEW_DASH_HPP_INCLUDED
#define BOOST_NIJI_VIEW_DASH_HPP_INCLUDED

#include <type_traits>
#include <boost/assert.hpp>
#include <boost/range/iterator.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/view.hpp>
#include <boost/niji/support/utility/as.hpp>
#include <boost/niji/view/detail/dash.hpp>

namespace boost { namespace niji
{
    template<class T, class Pattern>
    struct dash_view : view<dash_view<T, Pattern>>
    {
        template<class Path, class...>
        using point_type = point<T>;
        
        Pattern pattern;
        
        template<class Sink>
        struct adaptor
        {
            void operator()(move_to_t, point<T> const& pt)
            {
                _dasher.move_to(pt);
            }
    
            void operator()(line_to_t, point<T> const& pt)
            {
                _dasher.line(pt, _sink);
            }
    
            void operator()(quad_to_t, point<T> const& pt1, point<T> const& pt2)
            {
                _dasher.quad(pt1, pt2, _sink);
            }
    
            void operator()(cubic_to_t, point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
            {
                // TODO
                BOOST_ASSERT(!"feature not implemented yet");
            }
    
            void operator()(end_poly_t)
            {
                _dasher.close(_sink);
            }
    
            void operator()(end_line_t)
            {
                _dasher.cut(_sink);
            }
            
            using iterator_t = typename range_iterator<Pattern const>::type;
    
            Sink& _sink;
            detail::dasher<T, iterator_t> _dasher;
        };
        
        dash_view() = default;
        
        explicit dash_view(Pattern pattern)
          : pattern(std::forward<Pattern>(pattern))
        {}

        template<class Path, class Sink>
        void iterate(Path const& path, Sink& sink) const
        {
            auto i = std::begin(pattern), e = std::end(pattern);
            if (i != e)
                niji::iterate(path, adaptor<Sink>{sink, {i, e}});
        }
        
        template<class Path, class Sink>
        void reverse_iterate(Path const& path, Sink& sink) const
        {
            iterate(path, sink);
        }
    };

    template<class Path, class T, class Pattern>
    struct path_cache<path_view<Path, dash_view<T, Pattern>>>
      : default_path_cache<path_view<Path, dash_view<T, Pattern>>>
    {};
}}

namespace boost { namespace niji { namespace views
{
    template<class T, class Pattern>
    inline auto dash(Pattern&& p)
    {
        return dash_view<T, Pattern>{std::forward<Pattern>(p)};
    }
    
    template<class T>
    inline auto dash(std::initializer_list<as_t<T>> const& p)
    {
        return dash_view<T, std::initializer_list<T> const&>{p};
    }
}}}

#endif
