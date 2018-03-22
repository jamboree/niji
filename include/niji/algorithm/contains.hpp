/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2018 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_ALGORITHM_CONTAINS_HPP_INCLUDED
#define NIJI_ALGORITHM_CONTAINS_HPP_INCLUDED

#include <niji/render.hpp>
#include <niji/support/command.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/point.hpp>
#include <niji/support/bezier.hpp>

namespace niji { namespace detail
{
    template<class T>
    inline T poly_eval(T A, T B, T C, T t)
    {
        return (A * t + B) * t + C;
    }

    template<class T>
    inline T poly_eval(T A, T B, T C, T D, T t)
    {
        return poly_eval(A, B, C, t) * t + D;
    }

    template<class T>
    T eval_cubic_pts(T c0, T c1, T c2, T c3, T t)
    {
        T A = c3 + 3 * (c1 - c2) - c0;
        T B = 3 * (c2 - c1 - c1 + c0);
        T C = 3 * (c1 - c0);
        T D = c0;
        return poly_eval(A, B, C, D, t);
    }

    template<class T>
    inline bool check_on_curve(T x, T y, const point<T>& start, const point<T>& end)
    {
        if (start.y == end.y)
            return numeric::between(start.x, x, end.x) && x != end.x;
        return x == start.x && y == start.y;
    }

    template<class T>
    int winding_line(const point<T> pts[2], point<T> pt, int& on_curve_count)
    {
        using std::swap;

        T x0 = pts[0].x;
        T y0 = pts[0].y;
        T x1 = pts[1].x;
        T y1 = pts[1].y;

        T dy = y1 - y0;

        int dir = 1;
        if (y0 > y1)
        {
            swap(y0, y1);
            dir = -1;
        }
        if (pt.y < y0 || pt.y > y1)
            return 0;

        if (check_on_curve(pt.x, pt.y, pts[0], pts[1]))
        {
            ++on_curve_count;
            return 0;
        }
        if (pt.y == y1)
            return 0;

        T cross = (x1 - x0) * (pt.y - pts[0].y) - dy * (pt.x - x0);

        if (!cross)
        {
            // Zero cross means the point is on the line, and since the case where
            // y of the query point is at the end point is handled above, we can be
            // sure that we're on the line (excluding the end point) here.
            if (pt.x != x1 || pt.y != pts[1].y)
                ++on_curve_count;
            dir = 0;
        }
        else if (numeric::get_sign(cross) == dir)
            dir = 0;

        return dir;
    }
    
    template<class T>
    int winding_mono_quad(const point<T> pts[3], point<T> pt, int& on_curve_count)
    {
        using std::swap;
        using std::abs;

        T y0 = pts[0].y;
        T y2 = pts[2].y;

        int dir = 1;
        if (y0 > y2)
        {
            swap(y0, y2);
            dir = -1;
        }
        if (pt.y < y0 || pt.y > y2)
            return 0;

        if (check_on_curve(pt.x, pt.y, pts[0], pts[2]))
        {
            ++on_curve_count;
            return 0;
        }
        if (pt.y == y2)
            return 0;

        T roots[2];
        auto end = find_unit_quad_roots
        (
            pts[0].y - 2 * pts[1].y + pts[2].y,
            2 * (pts[1].y - pts[0].y),
            pts[0].y - pt.y,
            roots
        );

        T xt;
        if (roots == end)
        {
            // Zero roots are returned only when y0 == y
            // Need [0] if dir == 1
            // and  [2] if dir == -1
            xt = pts[1 - dir].x;
        }
        else
        {
            T t = roots[0];
            T C = pts[0].x;
            T A = pts[2].x - 2 * pts[1].x + C;
            T B = 2 * (pts[1].x - C);
            xt = poly_eval(A, B, C, t);
        }
        if (numeric::is_nearly_zero(abs(xt - pt.x)))
        {
            if (pt.x != pts[2].x || pt.y != pts[2].y) // Don't test end points; they're start points.
            {
                ++on_curve_count;
                return 0;
            }
        }
        return xt < pt.x ? dir : 0;
    }

    template<class T>
    int winding_quad(const point<T> pts[3], point<T> pt, int& on_curve_count)
    {
        point<T> dst[5];
        int n = 0;

        if (!bezier::is_mono_quad(pts[0].y, pts[1].y, pts[2].y))
        {
            n = bezier::chop_quad_at_extrema<1>(pts, dst);
            pts = dst;
        }
        int w = winding_mono_quad(pts, pt, on_curve_count);
        if (n > 0)
            w += winding_mono_quad(&pts[2], pt, on_curve_count);

        return w;
    }

    template<int I, class T>
    bool chop_mono_cubic_at_coord(const point<T> pts[4], T c, T& t)
    {
        T crv[4] =
        {
            pts[0].coord<I>() - c,
            pts[1].coord<I>() - c,
            pts[2].coord<I>() - c,
            pts[3].coord<I>() - c
        };

        // Newton¡VRaphson method.
        // Quadratic convergence, tcpicallc <= 3 iterations.

        // Initial guess.
        // TODO(turk): Check for zero denominator? Shouldn't happen unless the curve
        // is not onlc monotonic but degenerate.
        T t1 = crv[0] / (crv[0] - crv[3]);

        // Newton's iterations.
        const T tol = T(1) / 16384;  // This leaves 2 fixed noise bits.
        T t0;
        const int maxiters = 5;
        int iters = 0;
        bool converged;
        do
        {
            t0 = t1;
            T c01 = numeric::interpolate(crv[0], crv[1], t0);
            T c12 = numeric::interpolate(crv[1], crv[2], t0);
            T c23 = numeric::interpolate(crv[2], crv[3], t0);
            T c012 = numeric::interpolate(c01, c12, t0);
            T c123 = numeric::interpolate(c12, c23, t0);
            T c0123 = numeric::interpolate(c012, c123, t0);
            T der = (c123 - c012) * 3;
            // TODO(turk): check for der==0: horizontal(I==1)/vertical(I==0).
            t1 -= c0123 / der;
            converged = abs(t1 - t0) <= tol;  // NaN-safe
            ++iters;
        } while (!converged && (iters < maxiters));
        t = t1; // Return the result.

        // The result might be valid, even if outside of the range [0, 1], but
        // we never evaluate a Bezier outside this interval, so we return false.
        if (t1 < 0 || t1 > 1)
            return false; // This shouldn't happen, but check anyway.
        return converged;
    }

    template<class T>
    int winding_mono_cubic(const point<T> pts[4], point<T> pt, int& on_curve_count)
    {
        using std::swap;
        using std::abs;

        T y0 = pts[0].y;
        T y3 = pts[3].y;

        int dir = 1;
        if (y0 > y3)
        {
            swap(y0, y3);
            dir = -1;
        }
        if (pt.y < y0 || pt.y > y3)
            return 0;

        if (check_on_curve(pt.x, pt.y, pts[0], pts[3]))
        {
            ++on_curve_count;
            return 0;
        }
        if (pt.y == y3)
            return 0;

        // Quickreject or quickaccept.
        auto minmax = std::minmax({pts[0].x, pts[1].x, pts[2].x, pts[3].x});
        if (pt.x < minmax.first)
            return 0;

        if (pt.x > minmax.second)
            return dir;

        // Compute the actual x(t) value.
        T t;
        if (!chop_mono_cubic_at_coord<1>(pts, pt.y, t))
            return 0;

        T xt = eval_cubic_pts(pts[0].x, pts[1].x, pts[2].x, pts[3].x, t);
        if (numeric::is_nearly_zero(abs(xt - pt.x)))
        {
            if (pt.x != pts[3].x || pt.y != pts[3].y) // Don't test end points; they're start points.
            {
                ++on_curve_count;
                return 0;
            }
        }
        return xt < pt.x ? dir : 0;
    }

    template<class T>
    int winding_cubic(const point<T> pts[4], point<T> pt, int& on_curve_count)
    {
        point<T> dst[10];
        int n = bezier::chop_cubic_at_extrema<1>(pts, dst);
        int w = 0;
        for (int i = 0; i <= n; ++i)
            w += winding_mono_cubic(&dst[i * 3], pt, on_curve_count);

        return w;
    }

    template<class T>
    struct contains_sink
    {
        int winding = 0;
        int on_curve_count = 0;

        contains_sink(point<T> pt) : _pt(pt) {}

        void operator()(move_to_t, point<T> const& pt)
        {
            _first = _pt0 = pt;
        }

        void operator()(line_to_t, point<T> const& pt1)
        {
            point<T> pts[2] = {_pt0, pt1};
            winding += winding_line(pts, _pt, on_curve_count);
            _pt0 = pt1;
        }

        void operator()(quad_to_t, point<T> const& pt1, point<T> const& pt2)
        {
            point<T> pts[3] = {_pt0, pt1, pt2};
            winding += winding_quad(pts, _pt, on_curve_count);
            _pt0 = pt2;
        }

        void operator()(cubic_to_t, point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
        {
            point<T> pts[4] = {_pt0, pt1, pt2, pt3};
            winding += winding_cubic(pts, _pt, on_curve_count);
            _pt0 = pt3;
        }

        void operator()(end_open_t) {}

        void operator()(end_closed_t)
        {
            operator()(line_to_t{}, _first);
        }

    private:
        point<T> _pt, _pt0, _first;
    };
}}

namespace niji
{
    template<class Path>
    bool contains(Path const& path, path_point_t<Path> pt)
    {
        using coord_t = path_coordinate_t<Path>;
        detail::contains_sink<coord_t> test{pt};
        niji::render(path, test);
        if (test.winding)
            return true;
        if (test.on_curve_count <= 1)
            return !!test.on_curve_count;
        return !!(test.on_curve_count & 1);
        // TODO:
        // If the point touches an even number of curves, and the fill is winding, check for
        // coincidence. Count coincidence as places where the on curve points have identical tangents.
    }
}

#endif