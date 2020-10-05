/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_ROTATE_HPP_INCLUDED
#define NIJI_VIEW_ROTATE_HPP_INCLUDED

#include <niji/view/transform.hpp>
#include <niji/support/transform/rotate.hpp>

namespace niji::views
{
    template<class T>
    using rotate = transform_view<transforms::rotate<T>>;
}

#endif