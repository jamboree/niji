/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_POINT_HPP_INCLUDED
#define NIJI_SUPPORT_POINT_HPP_INCLUDED

#include <cassert>
#include <type_traits>
#include <niji/support/index_constant.hpp>
#include <niji/support/is_narrowing.hpp>

namespace niji
{
    template<class T>
    struct impl_point;

    template<class T>
    concept Point = requires(T self)
    {
        impl_point<T>{};
    };

    template<Point T>
    struct point_coordinate
    {
        using type = typename impl_point<T>::coordinate_type;
    };

    template<Point T>
    using point_coordinate_t = typename point_coordinate<T>::type;

    template<Point P>
    inline decltype(auto) get_x(P const& p) { return impl_point<P>::get_x(p); }

    template<Point P>
    inline decltype(auto) get_y(P const& p) { return impl_point<P>::get_y(p); }

    template<class T>
    struct point
    {
        using coordinate_type = T;

        T x, y;
        
        constexpr point() : x(), y() {}

        constexpr point(T x, T y) : x(x), y(y) {}

        template<Point P>
        constexpr explicit(is_narrowing_v<point_coordinate_t<P>, T>) point(P const& other)
            : x(static_cast<T>(get_x(other))), y(static_cast<T>(get_y(other)))
        {}

        void reset(T x2, T y2)
        {
            x = std::move(x2);
            y = std::move(y2);
        }

        constexpr T& coord(index_constant<0>) { return x; }
        constexpr T& coord(index_constant<1>) { return y; }
        constexpr T const& coord(index_constant<0>) const { return x; }
        constexpr T const& coord(index_constant<1>) const { return y; }

        template<class U>
        bool operator==(point<U> const& other) const
        {
            return x == other.x && y == other.y;
        }
        
        template<class U>
        bool operator!=(point<U> const& other) const
        {
            return !operator==(other);
        }
    
        point operator-() const
        {
            return point(-x, -y);
        }

        template<class U>
        point& operator+=(point<U> const& other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        template<class U>
        point& operator-=(point<U> const& other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        template<class U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
        point& operator*=(U const& val)
        {
            x *= val;
            y *= val;
            return *this;
        }

        template<class U>
        point& operator*=(point<U> const& other)
        {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        template<class U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
        point& operator/=(U const& val)
        {
            x /= val;
            y /= val;
            return *this;
        }
        
        template<class U>
        point& operator/=(point<U> const& other)
        {
            x /= other.x;
            y /= other.y;
            return *this;
        }
    };

    template<class T>
    struct impl_point<point<T>>
    {
        using coordinate_type = T;
        static T get_x(point<T> const& self) { return self.x; }
        static T get_y(point<T> const& self) { return self.y; }
    };

#define NIJI_DEFINE_POINT_OEPRATOR(op)                                          \
    template<class T, class U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>\
    inline point<std::common_type_t<T, U>> operator op(point<T> const& pt, U const& val)\
    {                                                                           \
        return point<std::common_type_t<T, U>>(pt.x op val, pt.y op val);       \
    }                                                                           \
    template<class T, class U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>\
    inline point<std::common_type_t<T, U>> operator op(U const& val, point<T> const& pt)\
    {                                                                           \
        return point<std::common_type_t<T, U>>(val op pt.x, val op pt.y);       \
    }                                                                           \
    template<class T, class U>                                                  \
    inline point<std::common_type_t<T, U>> operator op(point<T> const& lhs, point<U> const& rhs)\
    {                                                                           \
        return point<std::common_type_t<T, U>>(lhs.x op rhs.x, lhs.y op rhs.y); \
    }                                                                           \
/***/
    NIJI_DEFINE_POINT_OEPRATOR(+)
    NIJI_DEFINE_POINT_OEPRATOR(-)
    NIJI_DEFINE_POINT_OEPRATOR(*)
    NIJI_DEFINE_POINT_OEPRATOR(/)
#undef NIJI_DEFINE_POINT_OEPRATOR

    using ipoint = point<int>;
    using fpoint = point<float>;
    using dpoint = point<double>;

    template<class T>
    struct radius : point<T>
    {
        using point<T>::point;

        radius(T value) : point<T>(value, value) {}

        radius& operator=(T value)
        {
            this->x = value;
            this->xy = value;
            return *this;
        }
    };
}

namespace niji::points
{
    template<class T, class F>
    inline point<T> transform(point<T> const& a, point<T> const& b, F&& f)
    {
        return {f(a.x, b.x), f(a.y, b.y)};
    }
    
    template<class T>
    inline point<T> middle(point<T> const& a, point<T> const& b)
    {
        return transform(a, b, [](T a, T b) { return (a + b) / 2; });
    }
    
    template<class T>
    inline point<T> interpolate(point<T> const& a, point<T> const& b, T t)
    {
        return transform(a, b, [t](T a, T b) { return a + (b - a) * t; });
    }
}

#endif