/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_PRINTER_HPP_INCLUDED
#define BOOST_NIJI_PRINTER_HPP_INCLUDED

#include <iostream>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/traits.hpp>

namespace boost { namespace niji
{
    struct printer
    {
        std::ostream& out;
        
        explicit printer(std::ostream& out)
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
            void operator()(Point const& pt) const
            {
                using boost::geometry::get;

                out << " (" << get<0>(pt) << ", " << get<1>(pt) << ')';
            }
        };
        
        template<class Point>
        void operator()(tag_str tag, Point const& pt) const
        {
            using boost::geometry::get;
            
            out << tag.str;
            (print_pt(out))(pt);
            out << std::endl;
        }

        template<class Point>
        void operator()(quad_to_t, Point const& pt1, Point const& pt2) const
        {
            out << "quad_to";
            print_pt print(out);
            print(pt1), print(pt2);
            out << std::endl;
        }
        
        template<class Point>
        void operator()(cubic_to_t, Point const& pt1, Point const& pt2, Point const& pt3) const
        {
            out << "cubic_to";
            print_pt print(out);
            print(pt1), print(pt2), print(pt3);
            out << std::endl;
        }
 
        void operator()(end_line_t) const
        {
            out << "end_line\n";
        }

        void operator()(end_poly_t) const
        {
            out << "end_poly\n";
        }
    };
}}

#endif
