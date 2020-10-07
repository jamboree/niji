/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_ANY_HPP_INCLUDED
#define NIJI_SINK_ANY_HPP_INCLUDED

namespace niji::detail
{
    template<class T, class Point>
    struct sink_impl
    {
        static void move_to(void* p, Point const& pt)
        {
            static_cast<T*>(p)->move_to(pt);
        }

        static void line_to(void* p, Point const& pt)
        {
            static_cast<T*>(p)->line_to(pt);
        }

        static void quad_to(void* p, Point const& pt1, Point const& pt2)
        {
            static_cast<T*>(p)->quad_to(pt1, pt2);
        }

        static void cubic_to(void* p, Point const& pt1, Point const& pt2, Point const& pt3)
        {
            static_cast<T*>(p)->cubic_to(pt1, pt2, pt3);
        }

        static void end_closed(void* p)
        {
            static_cast<T*>(p)->end_closed();
        }

        static void end_open(void* p)
        {
            static_cast<T*>(p)->end_open();
        }
    };

    template<class Point>
    struct sink_vtable
    {
        void(*move_to)(void*, Point const& pt);
        void(*line_to)(void*, Point const& pt);
        void(*quad_to)(void*, Point const& pt1, Point const& pt2);
        void(*cubic_to)(void*, Point const& pt1, Point const& pt2, Point const& pt3);
        void(*end_closed)(void*);
        void(*end_open)(void*);

        template<class Impl>
        constexpr sink_vtable(Impl)
            : move_to(Impl::move_to)
            , line_to(Impl::line_to)
            , quad_to(Impl::quad_to)
            , cubic_to(Impl::cubic_to)
            , end_closed(Impl::end_closed)
            , end_open(Impl::end_open)
        {}
    };

    template<class T, class Point>
    constexpr sink_vtable<Point> sink_vt = sink_impl<T, Point>{};
}

namespace niji
{
    template<class Point>
    struct any_sink
    {
        template<class Sink>
        any_sink(Sink& sink)
          : _sink(&sink)
          , _vptr(&detail::sink_vt<Sink, Point>)
        {}

        void move_to(Point const& pt)
        {
            _vptr->move_to(_sink, pt);
        }

        void line_to(Point const& pt)
        {
            _vptr->line_to(_sink, pt);
        }

        void quad_to(Point const& pt1, Point const& pt2)
        {
            _vptr->quad_to(_sink, pt1, pt2);
        }

        void cubic_to(Point const& pt1, Point const& pt2, Point const& pt3)
        {
            _vptr->cubic_to(_sink, pt1, pt2, pt3);
        }

        void end_open()
        {
            _vptr->end_open(_sink);
        }

        void end_closed()
        {
            _vptr->end_closed(_sink);
        }

    private:
        void* _sink;
        detail::sink_vtable<Point> const* _vptr;
    };
}

#endif