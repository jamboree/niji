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
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/view.hpp>

namespace boost { namespace niji
{
    struct repeat_view : view<repeat_view>
    {
        template<class Path, class...>
        using point_type = point<path_coordinate_t<Path>>;
                
        template<class Path, class Sink, class T>
        static void iterate(Path const& path, Sink& sink, std::size_t n, vector<T> const& v)
        {
             iterate_1d(path, sink, n, v.x, v.y, views::identity);
        }
        
        template<class Path, class Sink, class T>
        static void iterate(Path const& path, Sink& sink, std::size_t nx, std::size_t ny, T dx, T dy)
        {
             iterate_2d(path, sink, nx, ny, dx, dy, views::identity);
        }
                
        template<class Path, class Sink, class T>
        static void reverse_iterate(Path const& path, Sink& sink, std::size_t n, vector<T> const& v)
        {
             iterate_1d(path, sink, n, v.x, v.y, views::reverse);
        }
        
        template<class Path, class Sink, class T>
        static void reverse_iterate(Path const& path, Sink& sink, std::size_t nx, std::size_t ny, T dx, T dy)
        {
             iterate_2d(path, sink, nx, ny, dx, dy, views::reverse);
        }
        
    private:
        
        template<class Path, class Sink, class T, class View>
        static void iterate_1d(Path const& path, Sink& sink, std::size_t n, T dx, T dy, View view)
        {
            using coord_t = path_coordinate_t<Path>;
            
            path_cache<Path> cache;
            views::translate<coord_t> translate(0, 0);
            auto transformed = cache(path) | view | translate;

            for (std::size_t i = 0; i != n; ++i)
            {
                transformed.iterate(sink);
                translate.dx += dx;
                translate.dy += dy;
            }
        }
        
        template<class Path, class Sink, class T, class View>
        static void iterate_2d(Path const& path, Sink& sink, std::size_t nx, std::size_t ny, T dx, T dy, View view)
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
                    translate.dx += dx;
                }
                translate.dx = 0;
                translate.dy += dy;
            }
        }
    };
}}

namespace boost { namespace niji { namespace views
{
    constexpr repeat_view repeat {};
}}}

#endif
