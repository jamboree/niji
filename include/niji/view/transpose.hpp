/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_TRANSPOSE_HPP_INCLUDED
#define NIJI_VIEW_TRANSPOSE_HPP_INCLUDED

#include <niji/view/transform.hpp>
#include <niji/support/transform/transpose.hpp>

namespace niji { namespace views
{
    using transpose = transform_view<transforms::transpose>;
}}

#endif