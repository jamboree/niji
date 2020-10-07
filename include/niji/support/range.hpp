/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_RANGE_HPP_INCLUDED
#define NIJI_SUPPORT_RANGE_HPP_INCLUDED

#include <type_traits>
#include <iterator>

namespace niji::ranges
{
    namespace std_hack
    {
        using std::begin;
        using std::end;

        struct begin_fn
        {
            template<class T>
            auto operator()(T const& seq) const -> decltype(begin(seq))
            {
                return begin(seq);
            }
        };

        struct end_fn
        {
            template<class T>
            auto operator()(T const& seq) const -> decltype(end(seq))
            {
                return end(seq);
            }
        };
    }

    constexpr std_hack::begin_fn begin{};
    constexpr std_hack::end_fn end{};

    template<class T>
    concept Range = requires(T& t)
    {
        begin(t);
        end(t);
    };

    template<class T>
    using iterator_t = decltype(begin(std::declval<T&>()));

    template<class T>
    using sentinel_t = decltype(end(std::declval<T&>()));

    namespace detail
    {
        template<class T>
        struct iter_value
        {
            using type = std::decay_t<decltype(*std::declval<T>())>;
        };

        template<class T> requires requires { std::iterator_traits<std::remove_cvref_t<T>>::value_type; }
        struct iter_value<T>
        {
            using type = typename std::iterator_traits<std::remove_cvref_t<T>>::value_type;
        };
    }

    template<class T>
    using iter_value_t = typename detail::iter_value<T>::type;

    template<Range R>
    using range_value_t = iter_value_t<iterator_t<R>>;
}

#endif