/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2018 Jamboree

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
    template<class T, class Pattern, class U>
    struct dash_view : view<dash_view<T, Pattern, U>>
    {
        template<class Path>
        using point_type = point<T>;
        
        Pattern pattern;
        T offset;
        U weight;
        
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
    
            void operator()(end_closed_t)
            {
                _dasher.close(_sink);
            }
    
            void operator()(end_open_t)
            {
                _dasher.cut(_sink);
            }
            
            using iterator_t = typename
                boost::range_iterator<std::remove_reference_t<Pattern> const>::type;
    
            Sink& _sink;
            detail::dasher<T, U, iterator_t> _dasher;
        };
        
        dash_view() = default;
        
        explicit dash_view(Pattern pattern, T offset = {}, U weight = {})
          : pattern(std::forward<Pattern>(pattern))
          , offset(offset), weight(weight)
        {}

        template<class Path, class Sink>
        void render(Path const& path, Sink& sink) const
        {
            auto i = std::begin(pattern), e = std::end(pattern);
            if (i != e)
                niji::render(path, adaptor<Sink>{sink, {i, e, offset, weight}});
        }
        
        template<class Path, class Sink>
        void inverse_render(Path const& path, Sink& sink) const
        {
            render(path, sink);
        }
    };
}

namespace niji { namespace views
{
    template<class T, class Pattern = std::initializer_list<T> const&, class U = one>
    inline auto dash(Pattern&& p, just_t<T> offset = {}, U weight = {})
    {
        return dash_view<T, Pattern, U>{std::forward<Pattern>(p), offset, weight};
    }
}}

#endif