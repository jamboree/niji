/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

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
    template<class Sink>
    struct adaptor
    {
        Sink& sink;

        template<class Tag, class Point>
        void operator()(Tag tag, Point const& pt) const
        {
            sink(tag, pt);
        }
    
        template<class Point>
        void operator()(niji::quad_to_t, Point const& pt1, Point const& pt2) const
        {
            using namespace niji::command;
            sink(line_to, pt1);
            sink(line_to, pt2);
        }
    
        template<class Point>
        void operator()(niji::cubic_to_t, Point const& pt1, Point const& pt2, Point const& pt3) const
        {
            using namespace niji::command;
            sink(line_to, pt1);
            sink(line_to, pt2);
            sink(line_to, pt3);
        }
        
        template<class Tag>
        void operator()(Tag tag) const
        {
            sink(tag);
        }
    };
    
    template<class Path, class Sink>
    void render(Path const& path, Sink& sink) const
    {
        niji::render(path, adaptor<Sink>{sink});
    }

    template<class Path, class Sink>
    void inverse_render(Path const& path, Sink& sink) const
    {
        niji::inverse_render(path, adaptor<Sink>{sink});
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
