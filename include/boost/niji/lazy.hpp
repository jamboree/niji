/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_LAZY_HPP_INCLUDED
#define BOOST_NIJI_LAZY_HPP_INCLUDED

#include <type_traits>
#include <boost/niji/iterate.hpp>
#include <boost/optional/optional.hpp>

namespace boost { namespace niji
{
    template<class F>
    struct lazy_path
    {
        using result_type = std::result_of_t<F()>;
        using point_type = path_point_t<result_type>;
        
        F f;

        template<class Sink>
        void iterate(Sink& sink) const
        {
            niji::iterate(f(), sink);
        }
        
        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            niji::reverse_iterate(f(), sink);
        }
    };
    
    template<class F>
    inline lazy_path<F> lazy(F&& f)
    {
        return {std::forward<F>(f)};
    }
    
    template<class F>
    struct path_cache<lazy_path<F>>
    {
        decltype(auto) operator()(lazy_path<F> const& path)
        {
            _ret.emplace(path.f());
            return _cache(_ret.get());
        }
        
    private:
        
        using result_t = typename lazy_path<F>::result_type;
        
        optional<result_t> _ret;
        path_cache<result_t> _cache;
    };
}}

#endif
