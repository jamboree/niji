/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_BOX_HPP_INCLUDED
#define NIJI_SUPPORT_BOX_HPP_INCLUDED

#include <niji/support/vector.hpp>

namespace niji
{
    template<class T>
    struct box
    {
        using coordinate_type = T;
        using point_type = point<T>;

        point<T> min_corner;
        point<T> max_corner;
        
        box() = default;
        
        box(T x1, T y1, T x2, T y2)
          : min_corner(point<T>(x1, y1))
          , max_corner(point<T>(x2, y2))
        {}

        box(point<T> const& min_corner, point<T> const& max_corner)
          : min_corner(min_corner)
          , max_corner(max_corner)
        {}

        void reset(T x1, T y1, T x2, T y2)
        {
            min_corner = point<T>(x1, y1);
            max_corner = point<T>(x2, y2);
        }

        void reset(point<T> const& min, point<T> const& max)
        {
            min_corner = min;
            max_corner = max;
        }

        coordinate_type width() const
        {
            return length(index<0>);
        }

        coordinate_type height() const
        {
            return length(index<1>);
        }
                
        template<int I>
        coordinate_type length(index_constant<I> i) const
        {
            return max_corner.coord(i) - min_corner.coord(i);
        }
        
        bool empty() const
        {
            return
                max_corner.x == min_corner.x ||
                max_corner.y == min_corner.y;
        }
        
        bool invalid() const
        {
            return
                min_corner.x > max_corner.x ||
                min_corner.y > max_corner.y;
        }
        
        void clear()
        {
            max_corner = point<T>();
            min_corner = point<T>();
        }

        void translate(vector<T> const& v)
        {
            translate(v.x, v.y);
        }

        void translate(T x, T y)
        {
            min_corner.x = min_corner.x + x;
            min_corner.y = min_corner.y + y;
            max_corner.x = max_corner.x + x;
            max_corner.y = max_corner.y + y;
        }

        void correct()
        {
            if (max_corner.x < min_corner.x)
            {
                T tmp = min_corner.x;
                min_corner.x = max_corner.x;
                max_corner.x = tmp;
            }
            if (max_corner.y < min_corner.y)
            {
                T tmp = min_corner.y;
                min_corner.y = max_corner.y;
                max_corner.y = tmp;
            }
        }

        template<class F>
        box transformed(F&& f) const
        {
            box ret(f(min_corner), f(max_corner));
            ret.correct();
            return ret;
        }

        void offset(vector<T> const& v)
        {
            offset(v.x, v.y);
        }

        void offset(T x, T y)
        {
            min_corner.x = min_corner.x - x;
            min_corner.y = min_corner.y - y;
            max_corner.x = max_corner.x + x;
            max_corner.y = max_corner.y + y;
        }

        void expand(box const& other)
        {
            expand_coord(index<0>, other.min_corner.x, other.max_corner.x);
            expand_coord(index<1>, other.min_corner.y, other.max_corner.y);
        }
        
        template<int I>
        void expand_coord(index_constant<I> i, T min, T max)
        {
            auto min_ = min_corner.coord(i);
            auto max_ = max_corner.coord(i);
            if (min_ == max_)
            {
                min_corner.coord(i) = min;
                max_corner.coord(i) = max;
                return;
            }
            if (min < min_)
                min_corner.coord(i) = min;
            if (max > max_)
                max_corner.coord(i) = max;
        }

        bool clip(box const& other)
        {
            return
                clip_coord(index<0>, other.min_corner.x, other.max_corner.x) &&
                clip_coord(index<1>, other.min_corner.y, other.max_corner.y);
        }

        template<int I>
        bool clip_coord(index_constant<I> i, T min, T max)
        {
            auto min_ = min_corner.coord(i);
            auto max_ = max_corner.coord(i);
            if (min_ >= max)
            {
                max_corner.coord(i) = min_;
                return false;
            }
            if (max_ <= min)
            {
                min_corner.coord(i) = max_;
                return false;
            }
            if (min_ < min)
                min_corner.coord(i) = min;
            if (max < max_)
                max_corner.coord(i) = max;
            return true;
        }

        point<T>& corner(index_constant<0>) { return min_corner; }
        point<T>& corner(index_constant<1>) { return max_corner; }
        point<T> const& corner(index_constant<0>) const { return min_corner; }
        point<T> const& corner(index_constant<1>) const { return max_corner; }

        point<T> center() const
        {
            point<T> pt;
            pt.x = (min_corner.x + max_corner.x) / 2;
            pt.y = (min_corner.y + max_corner.y) / 2;
            return pt;
        }

        template<class Sink>
        void iterate(Sink& sink) const
        {
            iterate_impl(sink, max_corner, min_corner);
        }

        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            iterate_impl(sink, min_corner, max_corner);
        }

    private:
        template<class Sink>
        void iterate_impl(Sink& sink, point<T> const& c1, point<T> const& c2) const
        {
            sink.move_to(min_corner);
            sink.line_to(point<T>(c1.x, c2.y));
            sink.line_to(max_corner);
            sink.line_to(point<T>(c2.x, c1.y));
            sink.end_closed();
        }
    };
}

#endif