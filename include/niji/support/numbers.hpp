/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_NUMBERS_HPP_INCLUDED
#define NIJI_SUPPORT_NUMBERS_HPP_INCLUDED

#include <numbers>

namespace niji
{
    struct constant_literal
    {
        char const* data;
        std::size_t size;
    };

    constexpr constant_literal operator"" _const(char const* str, std::size_t n)
    {
        return {str, n};
    }
};

namespace niji::numbers
{
    using namespace ::std::numbers;

    template<class T>
    constexpr T root_two = "1.41421356237309504880168872420969807856967187537694807317667973799073247846210703885038753432764157273501384623e+00"_const;

    template<>
    constexpr double root_two<double> = 1.414213562373095048801688724209698078e+00;

    template<>
    constexpr float root_two<float> = static_cast<float>(root_two<double>);

    template<class T>
    constexpr T tan_pi_over_8 = "4.14213562373095048801688724209698078569671875376948073176679737990732478462107038850387534327641572735013846231e-01"_const;

    template<>
    constexpr double tan_pi_over_8<double> = 4.142135623730950488016887242096980786e-01;

    template<>
    constexpr float tan_pi_over_8<float> = static_cast<float>(tan_pi_over_8<double>);

    template<class T>
    constexpr T root2_over_2 = "7.07106781186547524400844362104849039284835937688474036588339868995366239231053519425193767163820786367506923115e-01"_const;

    template<>
    constexpr double root2_over_2<double> = 7.071067811865475244008443621048490393e-01;

    template<>
    constexpr float root2_over_2<float> = static_cast<float>(root2_over_2<double>);

    template<class T>
    constexpr T cubic_arc_factor = "5.52284749830793398402251632279597438092895833835930764235572983987643304616142718467183379103522096980018461641e-01"_const;

    template<>
    constexpr double cubic_arc_factor<double> = 5.522847498307933984022516322795974381e-01;

    template<>
    constexpr float cubic_arc_factor<float> = static_cast<float>(cubic_arc_factor<double>);
}

#endif