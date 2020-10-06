/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_REPEAT_HPP_INCLUDED
#define NIJI_VIEW_REPEAT_HPP_INCLUDED

#include <niji/view/identity.hpp>
#include <niji/view/reverse.hpp>
#include <niji/view/translate.hpp>
#include <niji/support/view.hpp>
#include <niji/support/just.hpp>

namespace niji
{
    template<class T>
    struct repeat_view : view<repeat_view<T>>
    {
        template<class Path, class...>
        using point_type = point<path_coordinate_t<Path>>;
        
        std::size_t n;
        vector<T> v;
        
        repeat_view(std::size_t n, vector<T> const& v)
          : n(n), v(v)
        {}
   
        template<Path P, class Sink>
        void iterate(P const& path, Sink& sink) const
        {
             iterate_impl(path, sink, n, v.x, v.y, views::identity);
        }
  
        template<BiPath P, class Sink>
        void reverse_iterate(P const& path, Sink& sink) const
        {
             iterate_impl(path, sink, n, v.x, v.y, views::reverse);
        }

    private:
        template<class Path, class Sink, class View>
        static void iterate_impl(Path const& path, Sink& sink, std::size_t n, T dx, T dy, View view)
        {
            using coord_t = path_coordinate_t<Path>;

            views::translate<coord_t> translate(0, 0);
            auto transformed = path | view | translate;

            for (std::size_t i = 0; i != n; ++i)
            {
                transformed.iterate(sink);
                translate.x += dx;
                translate.y += dy;
            }
        }
    };
}

namespace niji::views
{
    template<class T>
    inline repeat_view<T> repeat(std::size_t n, vector<just_t<T>> const& v)
    {
        return {n, v};
    }
}

#endif