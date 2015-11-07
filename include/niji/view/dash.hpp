/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_DASH_HPP_INCLUDED
#define NIJI_VIEW_DASH_HPP_INCLUDED

#include <type_traits>
#include <boost/assert.hpp>
#include <boost/range/iterator.hpp>
#include <niji/support/view.hpp>
#include <niji/support/just.hpp>
#include <niji/view/detail/dash.hpp>

namespace niji
{
    template<class T, class Pattern>
    struct dash_view : view<dash_view<T, Pattern>>
    {
        template<class Path>
        using point_type = point<T>;
        
        Pattern pattern;
        T offset;
        
        template<class Sink>
        struct adaptor
        {
            void operator()(move_to_t, point<T> const& pt)
            {
                _dasher.move_to(pt);
            }
    
            void operator()(line_to_t, point<T> const& pt)
            {
                _dasher.line_to(pt, _sink);
            }
    
            void operator()(quad_to_t, point<T> const& pt1, point<T> const& pt2)
            {
                _dasher.quad_to(pt1, pt2, _sink);
            }
    
            void operator()(cubic_to_t, point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
            {
                _dasher.cubic_to(pt1, pt2, pt3, _sink);
            }
    
            void operator()(end_poly_t)
            {
                _dasher.close(_sink);
            }
    
            void operator()(end_line_t)
            {
                _dasher.cut(_sink);
            }
            
            using iterator_t = typename
                boost::range_iterator<std::remove_reference_t<Pattern> const>::type;
    
            Sink& _sink;
            detail::dasher<T, iterator_t> _dasher;
        };
        
        dash_view() = default;
        
        explicit dash_view(Pattern pattern, T offset = {})
          : pattern(std::forward<Pattern>(pattern)), offset(offset)
        {}

        template<class Path, class Sink>
        void render(Path const& path, Sink& sink) const
        {
            auto i = std::begin(pattern), e = std::end(pattern);
            if (i != e)
                niji::render(path, adaptor<Sink>{sink, {i, e, offset}});
        }
        
        template<class Path, class Sink>
        void inverse_render(Path const& path, Sink& sink) const
        {
            render(path, sink);
        }
    };

    template<class Path, class T, class Pattern>
    struct path_cache<path_adaptor<Path, dash_view<T, Pattern>>>
      : default_path_cache<path_adaptor<Path, dash_view<T, Pattern>>>
    {};
}

namespace niji { namespace views
{
    template<class T, class Pattern>
    inline auto dash(Pattern&& p, just_t<T> offset = {})
    {
        return dash_view<T, Pattern>{std::forward<Pattern>(p), offset};
    }
    
    template<class T>
    inline auto dash(std::initializer_list<just_t<T>> const& p, just_t<T> offset = {})
    {
        return dash_view<T, std::initializer_list<T> const&>{p, offset};
    }
}}

#endif