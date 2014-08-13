/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_VIEW_TRANSLATE_HPP_INCLUDED
#define BOOST_NIJI_VIEW_TRANSLATE_HPP_INCLUDED

#include <boost/niji/view/transform.hpp>
#include <boost/niji/support/transform/translate.hpp>

namespace boost { namespace niji { namespace views
{
    using transforms::translate;
}}}

namespace boost { namespace niji { namespace transforms
{
    template<class Path, class T>
    inline auto operator|(Path&& path, translate<T>&& d)
    {
        return std::forward<Path>(path) | views::transform(std::move(d));
    }
    
    template<class Path, class T>
    inline auto operator|(Path&& path, translate<T> const& d)
    {
        return std::forward<Path>(path) | views::transform(d);
    }
}}}

#endif
