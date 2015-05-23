/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_PATH_FWD_HPP_INCLUDED
#define NIJI_PATH_FWD_HPP_INCLUDED

#include <memory> // for allocator

namespace niji
{
    template<class Node, class Alloc = std::allocator<Node>>
    class path;
}

#endif