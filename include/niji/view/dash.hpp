/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_DASH_HPP_INCLUDED
#define NIJI_VIEW_DASH_HPP_INCLUDED

#include <type_traits>
#include <niji/support/view.hpp>
#include <niji/support/just.hpp>
#include <niji/support/numeric.hpp>
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
            void move_to(point<T> const& pt)
            {
                _dasher.move_to(pt);
            }
    
            void line_to(point<T> const& pt)
            {
                _dasher.line_to(pt, _sink);
            }
    
            void quad_to(point<T> const& pt1, point<T> const& pt2)
            {
                _dasher.quad_to(pt1, pt2, _sink);
            }
    
            void cubic_to(point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
            {
                _dasher.cubic_to(pt1, pt2, pt3, _sink);
            }
    
            void end_closed()
            {
                _dasher.close(_sink);
            }
    
            void end_open()
            {
                _dasher.cut(_sink);
            }
            
            using iterator_t = typename std::decay_t<Pattern>::const_iterator;
    
            Sink& _sink;
            detail::dasher<T, U, iterator_t> _dasher;
        };
        
        dash_view() = default;
        
        explicit dash_view(Pattern pattern, T offset = {}, U weight = {})
          : pattern(std::forward<Pattern>(pattern))
          , offset(offset), weight(weight)
        {}

        template<Path P, class Sink>
        void iterate(P const& path, Sink& sink) const
        {
            using std::begin;
            using std::end;
            auto i = begin(pattern), e = end(pattern);
            if (i != e)
                niji::iterate(path, adaptor<Sink>{sink, {i, e, offset, weight}});
        }
    };
}

namespace niji::views
{
    template<class T, class Pattern = std::initializer_list<T> const&, class U = numeric::one>
    inline auto dash(Pattern&& p, just_t<T> offset = {}, U weight = {})
    {
        return dash_view<T, Pattern, U>{std::forward<Pattern>(p), offset, weight};
    }
}

#endif