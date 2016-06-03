/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_DEBUG_HPP_INCLUDED
#define NIJI_SINK_DEBUG_HPP_INCLUDED

#include <iostream>
#include <boost/geometry/core/access.hpp>
#include <niji/support/command.hpp>
#include <niji/support/traits.hpp>

namespace niji
{
    struct debug_sink
    {
        std::ostream& out;
        
        explicit debug_sink(std::ostream& out)
          : out(out)
        {}
        
        struct tag_str
        {
            char const* str;
            
            tag_str(move_to_t)
              : str("move_to")
            {}
            
            tag_str(line_to_t)
              : str("line_to")
            {}
        };
        
        struct print_pt
        {
            std::ostream& out;
          
            explicit print_pt(std::ostream& out)
              : out(out)
            {}
              
            template<class Point>
            void operator()(Point const& pt)
            {
                using boost::geometry::get;

                out << " (" << get<0>(pt) << ", " << get<1>(pt) << ')';
            }
        };
        
        template<class Point>
        void operator()(tag_str tag, Point const& pt)
        {
            using boost::geometry::get;
            
            out << tag.str;
            (print_pt(out))(pt);
            out << std::endl;
        }

        template<class Point>
        void operator()(quad_to_t, Point const& pt1, Point const& pt2)
        {
            out << "quad_to";
            print_pt print(out);
            print(pt1), print(pt2);
            out << std::endl;
        }
        
        template<class Point>
        void operator()(cubic_to_t, Point const& pt1, Point const& pt2, Point const& pt3)
        {
            out << "cubic_to";
            print_pt print(out);
            print(pt1), print(pt2), print(pt3);
            out << std::endl;
        }
 
        void operator()(end_line_t)
        {
            out << "end_line\n";
        }

        void operator()(end_poly_t)
        {
            out << "end_poly\n";
        }
    };
}

#endif