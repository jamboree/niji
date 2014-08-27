/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
// Boost.Geometry (aka GGL, Generic Geometry Library)
// Copyright (c) 2011-2012 Barend Gehrels, Amsterdam, the Netherlands.

#include <fstream>
#include <deque>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/niji/adapted/polygon.hpp>
#include <boost/niji/view/affine.hpp>
#include <boost/niji/algorithm/bounds.hpp>
#include "svg.hpp"

int main()
{
    using namespace boost::niji;
    using point_type = boost::geometry::model::d2::point_xy<double>;
    using polygon = boost::geometry::model::polygon<point_type>;

    polygon green, blue;

    boost::geometry::read_wkt(
        "POLYGON((2 1.3,2.4 1.7,2.8 1.8,3.4 1.2,3.7 1.6,3.4 2,4.1 3,5.3 2.6,5.4 1.2,4.9 0.8,2.9 0.7,2 1.3)"
            "(4.0 2.0, 4.2 1.4, 4.8 1.9, 4.4 2.2, 4.0 2.0))", green);

    boost::geometry::read_wkt(
        "POLYGON((4.0 -0.5 , 3.5 1.0 , 2.0 1.5 , 3.5 2.0 , 4.0 3.5 , 4.5 2.0 , 6.0 1.5 , 4.5 1.0 , 4.0 -0.5))", blue);

    auto box = bounds(green);
    box.expand(bounds(blue));

    views::affine<double> trans;
    trans
        .translate(-box.min_corner.x, -box.min_corner.y)
        .scale(400.0 / box.width(), 400.0 / box.height())
        .flip_y().translate(0, 400);
    
    std::deque<polygon> output;
    auto output_svg = [&](auto file)
    {
        std::ofstream fout(file);
        svg::canvas canvas(fout, 400, 400);
        
        canvas.stroke({2, svg::rgb(153,204,0)});
        canvas.fill({svg::rgb(153,204,0), 0.5});
        canvas.draw(green | trans);
    
        canvas.stroke({2, svg::rgb(51,51,153)});
        canvas.fill({svg::rgb(51,51,153), 0.3});
        canvas.draw(blue | trans);

        svg::pen pen(4, svg::rgb(255,128,0));
        pen.dash = {1, 7};
        pen.cap = svg::cap_style::round;
        canvas.stroke(pen);
        svg::font font(10, "Arial");
        int i = 0;
        for (auto const& g : output)
        {
            canvas.fill(nullptr);
            canvas.draw(g | trans);
            canvas.fill(svg::rgb());
            auto pt = boost::geometry::return_centroid<point_type>(g);
            canvas.text(trans(pt), std::to_string(i++), font);
        }
    };
    
    boost::geometry::intersection(green, blue, output);
    output_svg("geometry_intersection.svg");
    output.clear();
    boost::geometry::union_(green, blue, output);
    output_svg("geometry_union.svg");
    output.clear();
    boost::geometry::difference(green, blue, output);
    output_svg("geometry_difference1.svg");
    output.clear();
    boost::geometry::difference(blue, green, output);
    output_svg("geometry_difference2.svg");
    output.clear();
    boost::geometry::sym_difference(blue, green, output);
    output_svg("geometry_sym_difference.svg");
    
    return 0;
}
