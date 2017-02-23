/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_TEE_HPP_INCLUDED
#define NIJI_VIEW_TEE_HPP_INCLUDED

#include <niji/render.hpp>
#include <niji/support/view.hpp>

namespace niji
{
    template<class RedirectSink>
    struct tee_view : view<tee_view<RedirectSink>>
    {
        RedirectSink sink;

        template<class Sink>
        struct adaptor
        {
            RedirectSink& _redirect;
            Sink& _sink;

            template<class Tag, class... Points>
            void operator()(Tag tag, Points const&... pts)
            {
                _redirect(tag, pts...);
                _sink(tag, pts...);
            }
        };

        explicit tee_view(RedirectSink sink)
          : sink(std::forward<RedirectSink>(sink))
        {}

        template<class Path, class Sink>
        void render(Path const& path, Sink& sink_) const
        {
            niji::render(path, adaptor<Sink>{sink, sink_});
        }

        template<class Path, class Sink>
        void inverse_render(Path const& path, Sink& sink_) const
        {
            niji::inverse_render(path, adaptor<Sink>{sink, sink_});
        }
    };
}

namespace niji { namespace views
{
    template<class RedirectSink>
    inline tee_view<RedirectSink> tee(RedirectSink&& sink)
    {
        return tee_view<RedirectSink>{std::forward<RedirectSink>(sink)};
    }
}}

#endif