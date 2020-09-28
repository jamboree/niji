/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_DEBUG_HPP_INCLUDED
#define NIJI_SINK_DEBUG_HPP_INCLUDED

#include <iostream>
#include <niji/support/point.hpp>

namespace niji
{
    struct debug_sink
    {
        std::ostream& out;
        
        explicit debug_sink(std::ostream& out) : out(out) {}

        template<class Point>
        void print(Point const& pt) const
        {
            using trait = impl_point<Point>;
            out << " (" << trait::get_x(pt) << ", " << trait::get_y(pt) << ')';
        }

        template<class Point>
        void move_to(Point const& pt1)
        {
            out << "move_to";
            print(pt1);
            out << '\n';
        }

        template<class Point>
        void line_to(Point const& pt1)
        {
            out << "line_to";
            print(pt1);
            out << '\n';
        }

        template<class Point>
        void quad_to(Point const& pt1, Point const& pt2)
        {
            out << "quad_to";
            print(pt1), print(pt2);
            out << '\n';
        }
        
        template<class Point>
        void cubic_to(Point const& pt1, Point const& pt2, Point const& pt3)
        {
            out << "cubic_to";
            print(pt1), print(pt2), print(pt3);
            out << '\n';
        }
 
        void end_open()
        {
            out << "end_open\n";
        }

        void end_closed()
        {
            out << "end_closed\n";
        }
    };
}

#endif