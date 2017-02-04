/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/

#include <fstream>
#include <niji/support/point.hpp>
#include <niji/view/translate.hpp>
#include "svg.hpp"

template<class T>
struct arrow
{
    using point_type = niji::point<T>;

    T r;

    explicit arrow(T r) : r(r) {}

    template<class Sink>
    void render(Sink& sink) const
    {
        using namespace niji::command;
        sink(move_to, point_type{});
        sink(line_to, point_type{-r, -r});
        sink(line_to, point_type{2 * r, 0});
        sink(line_to, point_type{-r, r});
        sink(end_closed);
    }
};

int main()
{  
    using namespace niji;

    std::ofstream fout("arrow.svg");
    svg::canvas canvas(fout, 500, 500);

    canvas.fill(svg::rgb());
    canvas.draw(arrow<double>(50) | views::translate<double>(250, 250));

    return 0;
}
