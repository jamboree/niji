/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_ANY_PATH_HPP_INCLUDED
#define BOOST_NIJI_ANY_PATH_HPP_INCLUDED

#include <type_traits>
#include <functional>
#include <boost/niji/iterate.hpp>
#include <boost/niji/support/command.hpp>

namespace boost { namespace niji { namespace detail
{
    template<class... Ts>
    struct vtable
    {
        vtable(...) {}
    };
    
    template<class T, class... Ts>
    struct vtable<T, Ts...> : vtable<Ts...>
    {
        template<class Gen>
        vtable(Gen gen) : vtable<Ts...>(gen), f(Gen::f) {}
    
        operator T*() const { return f; }
        
        T* f;
    };
    
    template<class Sink>
    struct vgen
    {
        template<class Tag, class... Points>
        static void f(void* sink, Tag tag, Points const&... pts)
        {
            (*static_cast<Sink*>(sink))(tag, pts...);
        }
    };
}}}

namespace boost { namespace niji
{
    template<class Point>
    class any_path
    {
        struct any_sink
        {
            template<class Sink>
            any_sink(Sink& sink)
              : _sink(&sink)
              , _f(detail::vgen<Sink>())
            {}

            template<class Tag, class... Points>
            void operator()(Tag tag, Points const&... pts) const
            {
                _f(_sink, tag, pts...);
            }
            
            void* _sink;
            detail::vtable
            <
                void(void*, move_to_t, Point const&)
              , void(void*, line_to_t, Point const&)
              , void(void*, quad_to_t, Point const&, Point const&)
              , void(void*, cubic_to_t, Point const&, Point const&, Point const&)
              , void(void*, end_line_t)
              , void(void*, end_poly_t)
            > _f;
        };
        
        template<class Path>
        struct holder
        {
            Path path;
            
            void operator()(any_sink& sink, bool positive) const
            {
                if (positive)
                    niji::iterate(path, sink);
                else
                    niji::reverse_iterate(path, sink);
            }
        };
        
        template<class Path>
        using requires_valid = enable_if_c_t<
            !std::is_same<Path, any_path>::value
          && sizeof(path_iterate<Path, any_sink>), bool>;
        
    public:
        
        using point_type = Point;
        
        any_path() : _f([](any_sink&, bool){}) {}

        template<class Path, requires_valid<std::decay_t<Path>> = true>
        any_path(Path&& path)
          : _f(holder<std::decay_t<Path>>{std::forward<Path>(path)})
        {}
        
        template<class Path, requires_valid<std::decay_t<Path>> = true>
        any_path(std::reference_wrapper<Path> path)
          : _f(holder<Path&>{path})
        {}

        void iterate(any_sink sink) const
        {
            _f(sink, true);
        }

        void reverse_iterate(any_sink sink) const
        {
            _f(sink, false);
        }
        
    private:
        
        std::function<void(any_sink&, bool)> _f;
    };
}}

#endif
