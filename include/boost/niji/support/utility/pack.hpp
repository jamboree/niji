/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_PACK_HPP_INCLUDED
#define BOOST_NIJI_PACK_HPP_INCLUDED

#include <type_traits>
#include <functional>
#include <boost/ref.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/type_traits/has_trivial_copy.hpp>

namespace boost { namespace niji { namespace detail
{
    template<class T>
    struct mover
    {
        mover(T const& val) : val(val) {}
    
        mover(T&& val) : val(std::move(val)) {}
    
        mover(mover const& other) = default;
    
        mover(mover&& other) = default; 
    
        mover(mover& other) : val(std::move(other.val)) {}
    
        operator T const&() const
        {
            return val; 
        }
    
        T val;
    };
    
    template<class T>
    using wrap_t = typename mpl::eval_if
        <
            std::is_reference<T>
          , mpl::identity<std::reference_wrapper<std::remove_reference_t<T>>>
          , mpl::if_c
            <
                std::is_move_constructible<T>::value
                    && !(std::is_copy_constructible<T>::value && boost::has_trivial_copy_constructor<T>::value)
              , mover<T>
              , T
            >
        >::type;

    template<class... Ts>
    auto pack_impl(wrap_t<Ts>... ts)
    {
        return [=](auto&& f)->decltype(auto)
        {
            return f(static_cast<std::add_lvalue_reference_t<Ts const>>(ts)...);
        };
    }
    
    template<class... Ts>
    using pack_impl_t = decltype(pack_impl<Ts...>(std::declval<Ts>()...));
    
    template<class F>
    void pack_for_each_impl(F& f) {}
    
    template<class F, class T, class... Ts>
    void pack_for_each_impl(F& f, T&& t, Ts&&... ts)
    {
        f(std::forward<T>(t));
        pack_for_each_impl(f, std::forward<Ts>(ts)...);
    }
}}}

namespace boost { namespace niji
{
    template<class... Ts>
    struct pack : detail::pack_impl_t<Ts...>
    {
        using base_type = detail::pack_impl_t<Ts...>;
        
        template<class... As>
        pack(As&&... as)
          : base_type(detail::pack_impl<Ts...>(std::forward<As>(as)...))
        {}
        
        pack(pack const&) = default;
        pack(pack &&) = default;
    };
    
    constexpr auto pack_for_each = [](auto&& f)
    {
        return [&](auto&&... ts)
        {
            detail::pack_for_each_impl(f, static_cast<decltype(ts)>(ts)...);
        };
    };
}}

#endif
