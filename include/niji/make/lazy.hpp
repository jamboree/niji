/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_MAKE_LAZY_HPP_INCLUDED
#define NIJI_MAKE_LAZY_HPP_INCLUDED

#include <type_traits>
#include <niji/render.hpp>

namespace niji
{
    template<class F>
    struct lazy_path
    {
        using result_type = std::result_of_t<F()>;
        using point_type = path_point_t<result_type>;
        
        F f;

        template<class Sink>
        void render(Sink& sink) const
        {
            niji::render(f(), sink);
        }
        
        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            niji::inverse_render(f(), sink);
        }
    };
    
    template<class F>
    inline lazy_path<F> make_lazy(F&& f)
    {
        return {std::forward<F>(f)};
    }
}

#endif