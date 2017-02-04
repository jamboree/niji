/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/

#include <fstream>
#include <niji/make/lazy.hpp>
#include <niji/make/composite.hpp>
#include <niji/support/point.hpp>
#include <niji/graphic/ellipse.hpp>
#include <niji/view/inverse.hpp>
#include "svg.hpp"

template<class T>
auto donut(niji::point<T> const& pt, T o, T i)
{
    using namespace niji;

    return make_lazy([=]
    {
        return make_composite
        (
            ellipse<T>(pt, o)
          , ellipse<T>(pt, i) | views::inverse
        );
    });
}

int main()
{  
    using namespace niji;

    std::ofstream fout("donut.svg");
    svg::canvas canvas(fout, 500, 500);

    canvas.fill(svg::rgb());
    canvas.draw(donut<double>({250, 250}, 100, 60));

    return 0;
}
