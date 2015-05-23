/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_COMMAND_HPP_INCLUDED
#define NIJI_SUPPORT_COMMAND_HPP_INCLUDED

#include <type_traits>
#include <niji/support/identifier.hpp>

namespace niji
{
    // the order is intentional
    enum end_tag : char {poly, line};
    
    template<end_tag which>
    using end_tag_t = std::integral_constant<end_tag, which>;

    using end_line_t = end_tag_t<end_tag::line>;
    using end_poly_t = end_tag_t<end_tag::poly>;

    struct move_to_t {};
    struct line_to_t {};
    struct curve_to_t {};
    
    template<unsigned n>
    struct nth_curve_to_t : curve_to_t {};
    
    using quad_to_t = nth_curve_to_t<2>;
    using cubic_to_t = nth_curve_to_t<3>;
}

namespace niji { namespace command
{
    NIJI_IDENTIFIER(end_line_t, end_line)
    NIJI_IDENTIFIER(end_poly_t, end_poly)
    NIJI_IDENTIFIER(move_to_t, move_to)
    NIJI_IDENTIFIER(line_to_t, line_to)
    NIJI_IDENTIFIER(quad_to_t, quad_to)
    NIJI_IDENTIFIER(cubic_to_t, cubic_to)
}}

#endif
