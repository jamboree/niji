/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

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
    enum end_tag : char
    {
        closed,
        open,
        poly [[deprecated("used 'closed' instead")]] = closed,
        line [[deprecated("used 'open' instead")]] = open
    };
    
    template<end_tag which>
    using end_tag_t = std::integral_constant<end_tag, which>;

    using end_closed_t = end_tag_t<end_tag::closed>;
    using end_open_t = end_tag_t<end_tag::open>;

    using end_poly_t [[deprecated("used 'end_closed_t' instead")]] = end_closed_t;
    using end_line_t [[deprecated("used 'end_open_t' instead")]] = end_open_t;

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
    NIJI_IDENTIFIER(end_closed_t, end_closed);
    NIJI_IDENTIFIER(end_open_t, end_open);
    NIJI_IDENTIFIER(end_closed_t, end_poly [[deprecated("used 'end_closed' instead")]]);
    NIJI_IDENTIFIER(end_open_t, end_line [[deprecated("used 'end_open' instead")]]);
    NIJI_IDENTIFIER(move_to_t, move_to);
    NIJI_IDENTIFIER(line_to_t, line_to);
    NIJI_IDENTIFIER(quad_to_t, quad_to);
    NIJI_IDENTIFIER(cubic_to_t, cubic_to);
}}

#endif