/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/

#include <fstream>
#include <boost/niji/lazy.hpp>
#include <boost/niji/compose.hpp>
#include <boost/niji/support/point.hpp>
#include <boost/niji/graphic/ellipse.hpp>
#include <boost/niji/view/reverse.hpp>
#include "svg.hpp"

template<class T>
auto donut(boost::niji::point<T> const& pt, T o, T i)
{
    using namespace boost::niji;

    return lazy([=]
    {
        return compose
        (
            ellipse<T>(pt, o)
          , ellipse<T>(pt, i) | views::reverse
        );
    });
}

int main()
{  
    using namespace boost::niji;

    std::ofstream fout("donut.svg");
    svg::canvas canvas(fout, 500, 500);

    canvas.fill(svg::rgb());
    canvas.draw(donut<double>({250, 250}, 100, 60));

    return 0;
}
