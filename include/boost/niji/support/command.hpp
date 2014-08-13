/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_COMMAND_HPP_INCLUDED
#define BOOST_NIJI_COMMAND_HPP_INCLUDED

#include <type_traits>

namespace boost { namespace niji
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
}}

namespace boost { namespace niji { namespace command
{
    constexpr end_line_t end_line {};
    constexpr end_poly_t end_poly {};
    constexpr move_to_t move_to {};
    constexpr line_to_t line_to {};
    constexpr quad_to_t quad_to {};
    constexpr cubic_to_t cubic_to {};
}}}

#endif
