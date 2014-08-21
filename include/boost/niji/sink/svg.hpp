/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_SINK_SVG_HPP_INCLUDED
#define BOOST_NIJI_SINK_SVG_HPP_INCLUDED

#include <iostream>
#include <boost/geometry/core/access.hpp>
#include <boost/niji/support/command.hpp>

namespace boost { namespace niji
{
    template<class Ostream>
    struct basic_svg_sink
    {
        Ostream& out;
        
        explicit basic_svg_sink(Ostream& out)
          : out(out)
        {}
        
        template<class Point>
        void operator()(move_to_t, Point const& pt) const
        {
            using geometry::get;
            out << 'M' << get<0>(pt) << ',' << get<1>(pt);
        }
        
        template<class Point>
        void operator()(line_to_t, Point const& pt) const
        {
            using geometry::get;
            out << 'L' << get<0>(pt) << ',' << get<1>(pt);
        }
        
        template<class Point>
        void operator()(quad_to_t, Point const& pt1, Point const& pt2) const
        {
            using geometry::get;
            out << 'Q' << get<0>(pt1) << ',' << get<1>(pt1)
                << ' ' << get<0>(pt2) << ',' << get<1>(pt2);
        }
        
        template<class Point>
        void operator()(cubic_to_t, Point const& pt1, Point const& pt2, Point const& pt3) const
        {
            using geometry::get;
            out << 'C' << get<0>(pt1) << ',' << get<1>(pt1)
                << ' ' << get<0>(pt2) << ',' << get<1>(pt2)
                << ' ' << get<0>(pt3) << ',' << get<1>(pt3);
        }

        void operator()(end_line_t) const {}
        
        void operator()(end_poly_t) const
        {
            out << 'Z';
        }
    };
    
    using svg_sink = basic_svg_sink<std::ostream>;
}}

#endif
