/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_PATH_REF_HPP_INCLUDED
#define NIJI_PATH_REF_HPP_INCLUDED

#include <type_traits>
#include <niji/core.hpp>
#include <niji/sink/any.hpp>

namespace niji::detail
{
    template<class T, class Point>
    struct forward_path_impl
    {
        static void iterate(void const* p, any_sink<Point>& sink)
        {
            niji::iterate(*static_cast<T const*>(p), sink);
        }
    };

    template<class T, class Point>
    struct bidirectional_path_impl : forward_path_impl<T, Point>
    {
        static void reverse_iterate(void const* p, any_sink<Point>& sink)
        {
            niji::reverse_iterate(*static_cast<T const*>(p), sink);
        }
    };

    template<class Point>
    struct forward_path_vtable
    {
        void(*iterate)(void const* p, any_sink<Point>& sink);

        template<class Impl>
        constexpr forward_path_vtable(Impl)
            : iterate(Impl::iterate)
        {}
    };

    template<class Point>
    struct bidirectional_path_vtable : forward_path_vtable<Point>
    {
        void(*reverse_iterate)(void const* p, any_sink<Point>& sink);

        template<class Impl>
        constexpr bidirectional_path_vtable(Impl impl)
            : forward_path_vtable<Point>(impl)
            , reverse_iterate(Impl::reverse_iterate)
        {}
    };

    template<class T, class Point>
    constexpr forward_path_vtable<Point> forward_path_vt = forward_path_impl<T, Point>{};

    template<class T, class Point>
    constexpr bidirectional_path_vtable<Point> bidirectional_path_vt = bidirectional_path_impl<T, Point>{};
}

namespace niji
{
    template<Point Pt>
    struct forward_path_ref
    {
        using point_type = Pt;

        template<Path P> requires (!std::is_base_of_v<forward_path_ref, P>)
        forward_path_ref(P const& path) noexcept
          : _p(&path), _vptr(&detail::forward_path_vt<P, Pt>)
        {}

        void iterate(any_sink<Pt> sink) const
        {
            _vptr->iterate(_p, sink);
        }

    protected:
        forward_path_ref(void const* p, detail::forward_path_vtable<Pt> const* vptr) noexcept
            : _p(p), _vptr(vptr)
        {}

        void const* _p;
        detail::forward_path_vtable<Pt> const* _vptr;
    };

    template<Point Pt>
    struct bidirectional_path_ref : forward_path_ref<Pt>
    {
        template<BiPath P>
        bidirectional_path_ref(P const& path) noexcept
            : forward_path_ref<Pt>(&path, &detail::bidirectional_path_vt<P, Pt>)
        {}

        void reverse_iterate(any_sink<Pt> sink) const
        {
            static_cast<detail::bidirectional_path_vtable<Point> const*>(this->_vptr)->reverse_iterate(this->_p, sink);
        }
    };
}

#endif