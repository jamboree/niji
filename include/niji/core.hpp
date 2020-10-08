/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_CORE_HPP_INCLUDED
#define NIJI_CORE_HPP_INCLUDED

#include <niji/support/point.hpp>

namespace niji::detail
{
    template<class Point>
    struct dummy_sink
    {
        void move_to(Point const& pt);
        void line_to(Point const& pt);
        void quad_to(Point const& pt1, Point const& pt2);
        void cubic_to(Point const& pt1, Point const& pt2, Point const& pt3);
        void end_closed();
        void end_open();
    };
};

namespace niji
{
    // The order is intentional.
    enum end_tag : char
    {
        closed,
        open
    };

    template<class T>
    struct impl_path;

    template<class T>
    struct impl_bi_path;

    template<class T>
    concept Path = requires(T self)
    {
        impl_path<T>{};
    };

    template<class T>
    concept BiPath = Path<T> && requires(T self)
    {
        impl_bi_path<T>{};
    };

    template<class T>
    concept Iteratable = requires(T const& self, detail::dummy_sink<typename T::point_type> sink)
    {
        self.iterate(sink);
    };

    template<class T>
    concept ReverseIteratable = Iteratable<T> && requires(T const& self, detail::dummy_sink<typename T::point_type> sink)
    {
        self.reverse_iterate(sink);
    };

    template<Iteratable T>
    struct impl_path<T>
    {
        using point_type = typename T::point_type;

        template<class Sink>
        static void iterate(T const& self, Sink& sink)
        {
            self.iterate(sink);
        }
    };

    template<ReverseIteratable T>
    struct impl_bi_path<T>
    {
        template<class Sink>
        static void reverse_iterate(T const& self, Sink& sink)
        {
            self.reverse_iterate(sink);
        }
    };

    template<Path T>
    struct path_point
    {
        using type = typename impl_path<T>::point_type;
    };

    template<Path T>
    using path_point_t = typename path_point<T>::type;

    template<Path T>
    struct path_coordinate
    {
        using type = point_coordinate_t<path_point_t<T>>;
    };

    template<Path T>
    using path_coordinate_t = typename path_coordinate<T>::type;
}

namespace niji::detail
{
    struct iterate_fn
    {
        template<Path P, class Sink>
        void operator()(P const& path, Sink&& sink) const
        {
            return impl_path<P>::iterate(path, sink);
        }
    };

    struct reverse_iterate_fn
    {
        template<BiPath P, class Sink>
        void operator()(P const& path, Sink&& sink) const
        {
            return impl_bi_path<P>::reverse_iterate(path, sink);
        }
    };
}

namespace niji
{
    constexpr detail::iterate_fn iterate{};
    constexpr detail::reverse_iterate_fn reverse_iterate{};
}

#endif