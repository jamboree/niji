/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2017-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_TEE_HPP_INCLUDED
#define NIJI_VIEW_TEE_HPP_INCLUDED

#include <niji/support/view.hpp>

namespace niji
{
    template<class RedirectSink>
    struct tee_view : view<tee_view<RedirectSink>>
    {
        RedirectSink& sink;

        template<class Sink, class Point>
        struct adaptor
        {
            RedirectSink& _redirect;
            Sink& _sink;

            void move_to(Point const& pt)
            {
                _redirect.move_to(pt);
                _sink.move_to(pt);
            }

            void line_to(Point const& pt)
            {
                _redirect.line_to(pt);
                _sink.line_to(pt);
            }

            void quad_to(Point const& pt1, Point const& pt2)
            {
                _redirect.quad_to(pt1, pt2);
                _sink.quad_to(pt1, pt2);
            }

            void cubic_to(Point const& pt1, Point const& pt2, Point const& pt3)
            {
                _redirect.cubic_to(pt1, pt2, pt3);
                _sink.cubic_to(pt1, pt2, pt3);
            }

            void end_closed()
            {
                _redirect.end_closed();
                _sink.end_closed();
            }

            void end_open()
            {
                _redirect.end_open();
                _sink.end_open();
            }
        };

        explicit tee_view(RedirectSink& sink) noexcept : sink(sink) {}

        template<Path P, class Sink>
        void iterate(P const& path, Sink& sink_) const
        {
            niji::iterate(path, adaptor<Sink, path_point_t<P>>{sink, sink_});
        }

        template<BiPath P, class Sink>
        void reverse_iterate(P const& path, Sink& sink_) const
        {
            niji::reverse_iterate(path, adaptor<Sink, path_point_t<P>>{sink, sink_});
        }
    };
}

namespace niji::views
{
    template<class RedirectSink>
    inline tee_view<RedirectSink> tee(RedirectSink& sink)
    {
        return tee_view<RedirectSink>{sink};
    }
}

#endif