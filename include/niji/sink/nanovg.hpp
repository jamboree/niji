/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_NANOVG_HPP_INCLUDED
#define NIJI_SINK_NANOVG_HPP_INCLUDED

#include <boost/geometry/core/access.hpp>
#include <niji/support/command.hpp>
#include <niji/algorithm/winding.hpp>
#include <nanovg.h>

namespace niji
{
    // This sink does not use winding, suitable for stroke-op.
    struct nanovg_sink
    {
        using point_t = point<float>;

        ::NVGcontext* context;

        explicit nanovg_sink(::NVGcontext* context)
          : context(context)
        {}

        void operator()(move_to_t, point_t const& pt)
        {
            ::nvgMoveTo(context, pt.x, pt.y);
        }

        void operator()(line_to_t, point_t const& pt)
        {
            ::nvgLineTo(context, pt.x, pt.y);
        }

        void operator()(quad_to_t, point_t const& pt1, point_t const& pt2)
        {
            ::nvgQuadTo(context, pt1.x, pt1.y, pt2.x, pt2.y);
        }

        void operator()(cubic_to_t, point_t const& pt1, point_t const& pt2, point_t const& pt3)
        {
            ::nvgBezierTo(context, pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y);
        }
        
        void operator()(end_line_t) {}

        void operator()(end_poly_t)
        {
            ::nvgClosePath(context);
        }
    };

    // This sink calculates winding on the fly, suitable for fill-op.
    struct nanovg_winding_sink
    {
        using point_t = point<float>;

        ::NVGcontext* context;

        explicit nanovg_winding_sink(::NVGcontext* context)
          : context(context)
        {}

        void operator()(move_to_t tag, point_t const& pt)
        {
            _winding(tag, pt);
            ::nvgMoveTo(context, pt.x, pt.y);
        }

        void operator()(line_to_t tag, point_t const& pt)
        {
            _winding(tag, pt);
            ::nvgLineTo(context, pt.x, pt.y);
        }

        void operator()(quad_to_t tag, point_t const& pt1, point_t const& pt2)
        {
            _winding(tag, pt1, pt2);
            ::nvgQuadTo(context, pt1.x, pt1.y, pt2.x, pt2.y);
        }

        void operator()(cubic_to_t tag, point_t const& pt1, point_t const& pt2, point_t const& pt3)
        {
            _winding(tag, pt1, pt2, pt3);
            ::nvgBezierTo(context, pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y);
        }
        
        void operator()(end_line_t tag) { _winding(tag); }

        void operator()(end_poly_t tag)
        {
            _winding(tag);
            ::nvgClosePath(context);
            ::nvgPathWinding(context, _winding.is_ccw ? NVG_SOLID : NVG_HOLE);
        }

    private:

        winding_sink<float> _winding;
    };
}

#endif