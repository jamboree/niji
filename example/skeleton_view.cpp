/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/

#include <fstream>
#include <niji/path.hpp>
#include <niji/support/point.hpp>
#include <niji/support/view.hpp>
#include "svg.hpp"

struct skeleton_view : niji::view<skeleton_view>
{
    template<class Sink, class Point>
    struct adaptor
    {
        Sink& sink;

        void move_to(Point const& pt)
        {
            sink.move_to(pt);
        }

        void line_to(Point const& pt)
        {
            sink.line_to(pt);
        }

        void quad_to(Point const& pt1, Point const& pt2) const
        {
            sink.line_to(pt1);
            sink.line_to(pt2);
        }

        void cubic_to(Point const& pt1, Point const& pt2, Point const& pt3) const
        {
            sink.line_to(pt1);
            sink.line_to(pt2);
            sink.line_to(pt3);
        }

        void end_open()
        {
            sink.end_open();
        }

        void end_closed()
        {
            sink.end_closed();
        }
    };

    template<niji::Path P, class Sink>
    void iterate(P const& path, Sink& sink) const
    {
        niji::iterate(path, adaptor<Sink, niji::path_point_t<P>>{sink});
    }

    template<niji::BiPath P, class Sink>
    void reverse_iterate(P const& path, Sink& sink) const
    {
        niji::reverse_iterate(path, adaptor<Sink, niji::path_point_t<P>>{sink});
    }
};

constexpr skeleton_view skeleton = {};

int main()
{  
    using namespace niji;

    std::ofstream fout("skeleton_view.svg");
    svg::canvas canvas(fout, 500, 500);
    
    path<dpoint> p;
    p.join_cubic({50, 50}, {150, 300}, {350, 0}, {500, 200});
    
    canvas.stroke({2, svg::rgb()});
    canvas.draw(p);
    
    canvas.stroke({2, svg::rgb(154, 206, 235)});
    canvas.draw(p | skeleton);
    return 0;
}
