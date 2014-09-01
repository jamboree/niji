/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_VIEW_REPEAT_HPP_INCLUDED
#define BOOST_NIJI_VIEW_REPEAT_HPP_INCLUDED

#include <boost/niji/view/identity.hpp>
#include <boost/niji/view/reverse.hpp>
#include <boost/niji/view/translate.hpp>
#include <boost/niji/support/view.hpp>
#include <boost/niji/support/utility/as.hpp>

namespace boost { namespace niji
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
   
        template<class Path, class Sink>
        void iterate(Path const& path, Sink& sink) const
        {
             iterate_impl(path, sink, n, v.x, v.y, views::identity);
        }
  
        template<class Path, class Sink>
        void reverse_iterate(Path const& path, Sink& sink) const
        {
             iterate_impl(path, sink, n, v.x, v.y, views::reverse);
        }

    private:
        
        template<class Path, class Sink, class View>
        static void iterate_impl(Path const& path, Sink& sink, std::size_t n, T dx, T dy, View view)
        {
            using coord_t = path_coordinate_t<Path>;
            
            path_cache<Path> cache;
            views::translate<coord_t> translate(0, 0);
            auto transformed = cache(path) | view | translate;

            for (std::size_t i = 0; i != n; ++i)
            {
                transformed.iterate(sink);
                translate.tx += dx;
                translate.ty += dy;
            }
        }
    };
    
    template<class T>
    struct repeat2_view : view<repeat2_view<T>>
    {
        template<class Path, class...>
        using point_type = point<path_coordinate_t<Path>>;
        
        std::size_t nx, ny;
        T dx, dy;
        
        repeat2_view(std::size_t nx, std::size_t ny, T dx, T dy)
          : nx(nx), ny(ny), dx(dx), dy(dy)
        {}

        template<class Path, class Sink>
        void iterate(Path const& path, Sink& sink) const
        {
             iterate_impl(path, sink, nx, ny, dx, dy, views::identity);
        }

        template<class Path, class Sink>
        void reverse_iterate(Path const& path, Sink& sink) const
        {
             iterate_impl(path, sink, nx, ny, dx, dy, views::reverse);
        }
        
    private:

        template<class Path, class Sink, class View>
        static void iterate_impl(Path const& path, Sink& sink, std::size_t nx, std::size_t ny, T dx, T dy, View view)
        {
            using coord_t = path_coordinate_t<Path>;
            
            path_cache<Path> cache;
            views::translate<coord_t> translate(0, 0);
            auto transformed = cache(path) | view | translate;

            for (std::size_t i = 0; i != ny; ++i)
            {
                for (std::size_t j = 0; j != nx; ++j)
                {
                    transformed.iterate(sink);
                    translate.tx += dx;
                }
                translate.tx = 0;
                translate.ty += dy;
            }
        }
    };
}}

namespace boost { namespace niji { namespace views
{
    template<class T>
    inline repeat_view<T> repeat(std::size_t n, vector<as_t<T>> const& v)
    {
        return {n, v};
    }
    
    template<class T>
    inline repeat2_view<T> repeat(std::size_t nx, std::size_t ny, as_t<T> dx, as_t<T> dy)
    {
        return {nx, ny, dx, dy};
    }
}}}

#endif
