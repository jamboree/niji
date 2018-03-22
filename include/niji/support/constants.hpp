/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2018 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_CONSTANTS_HPP_INCLUDED
#define NIJI_SUPPORT_CONSTANTS_HPP_INCLUDED

#include <boost/math/constants/constants.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

namespace boost { namespace math { namespace constants
{
    BOOST_DEFINE_MATH_CONSTANT(tan_pi_over_8, 4.142135623730950488016887242096980786e-01,  "4.14213562373095048801688724209698078569671875376948073176679737990732478462107038850387534327641572735013846231e-01")
    BOOST_DEFINE_MATH_CONSTANT(root2_over_2, 7.071067811865475244008443621048490393e-01, "7.07106781186547524400844362104849039284835937688474036588339868995366239231053519425193767163820786367506923115e-01")
    BOOST_DEFINE_MATH_CONSTANT(cubic_arc_factor, 5.522847498307933984022516322795974381e-01, "5.52284749830793398402251632279597438092895833835930764235572983987643304616142718467183379103522096980018461641e-01")
}}}

namespace boost { namespace math { namespace constants { namespace detail
{
    template<class T, int>
    struct sel_impl;

    template<class T>
    struct sel_impl<T, construct_from_float>
    {
        static BOOST_CONSTEXPR T apply(float const f, double const, long double const, const char*)
        {
            return f;
        }
    };

    template<class T>
    struct sel_impl<T, construct_from_double>
    {
        static BOOST_CONSTEXPR T apply(float const, double const d, long double const, const char*)
        {
            return d;
        }
    };

    template<class T>
    struct sel_impl<T, construct_from_long_double>
    {
        static BOOST_CONSTEXPR T apply(float const, double const, long double const ld, const char*)
        {
            return ld;
        }
    };

    template<class T>
    struct sel_impl<T, construct_from_string>
    {
        static T apply(float const, double const, long double const, const char* p)
        {
            return convert_from_string<T>(p);
        }
    };

    template<class T, class Policy = policies::policy<> >
    struct sel
      : sel_impl<T const, construction_traits<T, Policy>::type::value>
    {};
}}}}

#define NIJI_CONSTANT(T, val) ::boost::math::constants::detail::sel<T>::apply(BOOST_JOIN(val, F), val, BOOST_JOIN(val, L), #val)
#define NIJI_CONSTANTS_(_, T, i, val) BOOST_PP_COMMA_IF(i) NIJI_CONSTANT(T, val)
#define NIJI_CONSTANTS(T, seq) BOOST_PP_SEQ_FOR_EACH_I(NIJI_CONSTANTS_, T, seq)

namespace niji { namespace constants
{
    using namespace ::boost::math::constants;
}}

#endif