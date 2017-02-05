/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_DEPRECATE_HPP_INCLUDED
#define NIJI_SUPPORT_DEPRECATE_HPP_INCLUDED

#define NIJI_DEPRECATE_BY(alter) [[deprecated("used '"#alter"' instead")]]

#endif