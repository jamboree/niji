/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_SVG_HPP_INCLUDED
#define NIJI_SINK_SVG_HPP_INCLUDED

#include <iostream>

namespace niji
{
    template<class Ostream>
    struct basic_svg_sink
    {
        Ostream& out;
        
        explicit basic_svg_sink(Ostream& out)
          : out(out)
        {}
        
        template<class Point>
        void move_to(Point const& pt)
        {
            out << 'M' << get_x(pt) << ',' << get_y(pt);
        }
        
        template<class Point>
        void line_to(Point const& pt)
        {
            out << 'L' << get_x(pt) << ',' << get_y(pt);
        }
        
        template<class Point>
        void quad_to(Point const& pt1, Point const& pt2)
        {
            out << 'Q' << get_x(pt1) << ',' << get_y(pt1)
                << ' ' << get_x(pt2) << ',' << get_y(pt2);
        }
        
        template<class Point>
        void cubic_to(Point const& pt1, Point const& pt2, Point const& pt3)
        {
            out << 'C' << get_x(pt1) << ',' << get_y(pt1)
                << ' ' << get_x(pt2) << ',' << get_y(pt2)
                << ' ' << get_x(pt3) << ',' << get_y(pt3);
        }

        void end_open() {}
        
        void end_closed()
        {
            out << 'Z';
        }
    };
    
    using svg_sink = basic_svg_sink<std::ostream>;
}

#endif