/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_PATH_REF_HPP_INCLUDED
#define BOOST_NIJI_PATH_REF_HPP_INCLUDED

#include <type_traits>
#include <boost/niji/iterate.hpp>
#include <boost/niji/detail/any_sink.hpp>

namespace boost { namespace niji
{
    template<class Point>
    class path_ref
    {
        using sink_t = detail::any_sink<Point>;
        
        template<class Path>
        using requires_valid = enable_if_c_t<
            !std::is_same<Path, path_ref>::value
          && sizeof(path_iterate<Path, sink_t>), bool>;
        
        template<class Path>
        static void f(void const* p, sink_t& sink, bool positive)
        {
            auto& path = *static_cast<Path const*>(p);
            if (positive)
                niji::iterate(path, sink);
            else
                niji::reverse_iterate(path, sink);
        }
        
    public:
        
        using point_type = Point;

        template<class Path, requires_valid<Path> = true>
        path_ref(Path const& path)
          : _p(&path), _f(f<Path>)
        {}

        void iterate(sink_t sink) const
        {
            _f(_p, sink, true);
        }

        void reverse_iterate(sink_t sink) const
        {
            _f(_p, sink, false);
        }
        
    private:
        
        void const* _p;
        void(*_f)(void const*, sink_t&, bool);
    };
}}

#endif
