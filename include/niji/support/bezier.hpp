/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SUPPORT_BEZIER_HPP_INCLUDED
#define NIJI_SUPPORT_BEZIER_HPP_INCLUDED

#include <array>
#include <cmath>
#include <algorithm>
#include <boost/assert.hpp>
#include <niji/support/traits.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/point.hpp>
#include <niji/support/transform/affine.hpp>
#include <niji/support/transform/rotate.hpp>
#include <niji/support/math/constants.hpp>
#include <niji/support/math/functions.hpp>

// N O T E
// -------
// Most of the algorithms here are borrowed from Skia, see
// src/core/SkGeometry.cpp for more detail.
// Some are from Pomax's excellent article about bezier, see
// http://pomax.github.io/bezierinfo/.

namespace niji { namespace detail
{
    // Q = -1/2 (B + sign(B) sqrt[B*B - 4*A*C])
    // x1 = Q / A
    // x2 = C / Q
    template<class T>
    T* find_unit_quad_roots(T a, T b, T c, T roots[2])
    {
        using std::sqrt;
        using std::isnan;
        using std::swap;
    
        if (a == 0)
            return roots + valid_unit_divide(-c, b, *roots);
    
        T* r = roots;

        T root = b * b - 4 * a * c;
        if (root < 0 || isnan(root)) // complex roots
            return roots;

        root = sqrt(root);
    
        T q = (b < 0) ? (root - b) / 2 : -(b + root) / 2;
        r += valid_unit_divide(q, a, *r);
        r += valid_unit_divide(c, q, *r);
        if (r - roots == 2)
        {
            if (roots[0] > roots[1])
                swap(roots[0], roots[1]);
            else if (roots[0] == roots[1])  // nearly-equal?
                --r; // skip the double root
        }
        return r;
    }

    // Quad'(t) = At + B, where
    // A = 2(a - 2b + c)
    // B = 2(b - a)
    // Solve for t, only if it fits between 0 < t < 1
    template<class T>
    T* find_quad_extrema(T a, T b, T c, T tValues[1])
    {
        return tValues + valid_unit_divide(a - b, a - b - b + c, *tValues);    
    }
    
    // Cubic'(t) = At^2 + Bt + C, where
    // A = 3(-a + 3(b - c) + d)
    // B = 6(a - 2b + c)
    // C = 3(b - a)
    // Solve for t, keeping only those that fit betwee 0 < t < 1
    template<class T>
    T* find_cubic_extrema(T a, T b, T c, T d, T tValues[2])
    {
        // we divide A,B,C by 3 to simplify
        T A = d - a + 3 * (b - c);
        T B = 2 * (a - b - b + c);
        T C = b - a;
        return find_unit_quad_roots(A, B, C, tValues);
    }
    
    // Solve coeff(t) == 0, returning the number of roots that
    // lie withing 0 < t < 1.
    // coeff[0]t^3 + coeff[1]t^2 + coeff[2]t + coeff[3]
    //    
    // Eliminates repeated roots (so that all tValues are distinct, and are always
    // in increasing order.
    template<class T>
    T* solve_cubic_poly(T c0, T c1, T c2, T c3, T tValues[3])
    {
        using std::acos;
        using std::sqrt;
        using std::cos;
        using std::pow;
        using std::abs;

        if (!c0)
            return find_unit_quad_roots(c1, c2, c3, tValues);

        T a, b, c, Q, R;
        {
            a = c1 / c0;
            b = c2 / c0;
            c = c3 / c0;
        }
        T a2 = a * a;
        Q = (a2 - b * 3) / 9;
        R = (2 * a2 * a - 9 * a * b + 27 * c) / 54;
    
        T Q3 = Q * Q * Q;
        T R2MinusQ3 = R * R - Q3;
        T adiv3 = a / 3;
    
        T* roots = tValues;
        auto next_root = [&roots](T r)
        {
            if (0 < r && r < 1)
                *roots++ = r;
        };
    
        if (R2MinusQ3 < 0) // we have 3 real roots
        {
            T theta = acos(R / sqrt(Q3));
            T neg2RootQ = -2 * sqrt(Q);
    
            next_root(neg2RootQ * cos(theta / 3) - adiv3);
            next_root(neg2RootQ * cos((theta + constants::two_pi<T>()) / 3) - adiv3);
            next_root(neg2RootQ * cos((theta - constants::two_pi<T>()) / 3) - adiv3);
    
            // now sort the roots
            std::sort(tValues, roots);
            roots = std::unique(tValues, roots);
        }
        else // we have 1 real root
        {
            T A = abs(R) + sqrt(R2MinusQ3);
            A = pow(A, constants::third<T>());
            if (R > 0)
                A = -A;
            if (A != 0)
                A += Q / A;
            next_root(A - adiv3);
        }
    
        return roots;
    }

    // Looking for F' dot F'' == 0
    // 
    // A = b - a
    // B = c - 2b + a
    // C = d - 3c + 3b - a
    // 
    // F' = 3Ct^2 + 6Bt + 3A
    // F'' = 6Ct + 6B
    // 
    // F' dot F'' -> CCt^3 + 3BCt^2 + (2BB + CA)t + AB
    template<class T>
    T* find_cubic_max_curvature(point<T> const src[4], T tValues[3])
    {
        auto a = src[1] - src[0];
        auto b = src[2] - src[1] * 2 - src[0];
        auto c = src[3] + (src[1] - src[2]) * 3 - src[0];
        
        auto sum = [](point<T> const& pt)
        {
            return pt.x + pt.y;
        };

        T c0 = sum(c * c);
        T c1 = sum(b * c * 3);
        T c2 = sum(b * b * 2 + c * a);
        T c3 = sum(a * b);
        if (is_nearly_zero(c0)) // we're just a quadratic
            return find_unit_quad_roots(c1, c2, c3, tValues);
            
        auto it = solve_cubic_poly(c0, c1, c2, c3, tValues);
    
        // now remove extrema where the curvature is zero (mins)
        return std::remove_if(tValues, it, [](T t) {return !(0 < t && t < 1); });
    }

    template<int n, int k, class = void>
    struct binomial_coeff;

    template<>
    struct binomial_coeff<0, 0>
    {
        static constexpr int value = 1;
    };

    template<int n, int k>
    struct binomial_coeff<n, k, std::enable_if_t<(k <= n)>>
    {
        using zero = std::integral_constant<int, 0>;
        using lhs = std::conditional_t<k != 0, binomial_coeff<n - 1, k - 1>, zero>;
        using rhs = std::conditional_t<(k > n - 1), zero, binomial_coeff<n - 1, k>>;

        static constexpr int value = lhs::value + rhs::value;
    };

    template<class F>
    inline auto pack_for_each(F&& f)
    {
        return [&f](auto&... ts)
        {
            std::initializer_list<bool>{(f(ts), true)...};
        };
    }

    template<unsigned i, int n>
    struct derivative_impl
    {
        static constexpr int m = n - 1;

        template<class T, class... Ts>
        static T eval(T t, T t0, Ts... ts)
        {
            auto list = [=](auto&& f) mutable
            {
                return f(ts...);
            };
            list(pack_for_each([t0](T& t) mutable
            {
                T tt = t;
                t = m * (t - t0);
                t0 = tt;
            }));
            return list([t](Ts... ts)
            {
                return derivative_impl<i - 1, m>::eval(t, ts...);
            });
        }
    };

    template<int n>
    struct derivative_impl<0, n>
    {
        static constexpr int m = n - 1;

        template<class T, class... Ts>
        static T eval(T t, Ts... ts)
        {
            return loop<T>(t)(std::integral_constant<int, 0>(), ts...);
        }

        template<class T>
        struct loop
        {
            explicit loop(T t)
              : t(t), t2(1 - t), pow_t(1)
            {
                T *b = pow_t2, *e = b + n;
                for (T pow(1); e != b; )
                {
                    *--e = pow;
                    pow *= t2;
                }
            }

            T const t, t2;
            T pow_t;
            T pow_t2[n];

            template<int k, class... Ts>
            T operator()(std::integral_constant<int, k>, T t0, Ts... ts)
            {
                T ret(binomial_coeff<m, k>::value * pow_t2[k] * pow_t * t0);
                pow_t *= t;
                return ret + (*this)(std::integral_constant<int, k + 1>(), ts...);
            }

            T operator()(std::integral_constant<int, n>)
            {
                return 0;
            }
        };
    };

    template<unsigned i, int n, class T, class... Ts>
    T derivative(T t, Ts... ts)
    {
        return derivative_impl<i, n>::eval(t, ts...);
    }

    template<unsigned i, int n, class T, class... Ts>
    T derivative(T)
    {
        return 0;
    }

    // LGQ function for Bezier curve arc length
    template<class T, class... Points>
    T lgq(T t, Points const&... pts)
    {
        using std::sqrt;
        constexpr auto n = sizeof...(Points);
        T x = derivative<1, n>(t, pts.x...);
        T y = derivative<1, n>(t, pts.y...);
        return sqrt(x * x + y * y);
    }

    // Legendre-Gauss abscissae
    // (xi values, defined at i=n as the roots of the nth order Legendre
    // polynomial Pn(x))
    template<class T>
    struct lg_t
    {
        static std::array<T, 3> const& table(std::integral_constant<int, 3>)
        {
            static std::array<T, 3> const t =
            {{
                NIJI_CONSTANTS(T,
                    (0.5)
                    (0.1127016653792583114820734600217600389168)
                    (0.8872983346207416885179265399782399610832)
                )
            }};
            return t;
        }

        static std::array<T, 4> const& table(std::integral_constant<int, 4>)
        {
            static std::array<T, 4> const t =
            {{
                NIJI_CONSTANTS(T,
                    (0.3300094782075718675986671204483776563998)
                    (0.6699905217924281324013328795516223436002)
                    (0.06943184420297371238802675555359524745215)
                    (0.9305681557970262876119732444464047525478)
                )
            }};
            return t;
        }
    };

    // Legendre-Gauss weights
    // (wi values, defined by a function linked to in the Bezier primer article)
    template<class T>
    struct lg_c
    {
        static std::array<T, 3> const& table(std::integral_constant<int, 3>)
        {
            static std::array<T, 3> const t =
            {{
                NIJI_CONSTANTS(T,
                    (0.8888888888888888888888888888888888888888)
                    (0.5555555555555555555555555555555555555555)
                    (0.5555555555555555555555555555555555555555)
                )
            }};
            return t;
        }

        static std::array<T, 4> const& table(std::integral_constant<int, 4>)
        {
            static std::array<T, 4> const t =
            {{
                NIJI_CONSTANTS(T,
                    (0.6521451548625461426269360507780005927646)
                    (0.6521451548625461426269360507780005927646)
                    (0.3478548451374538573730639492219994072353)
                    (0.3478548451374538573730639492219994072353)
                )
            }};
            return t;
        }
    };
    
    template<class T, class... Points>
    T length_impl(Points const&... pts)
    {
        std::integral_constant<int, sizeof...(Points)> _n;
        auto c = lg_c<T>::table(_n).begin(),
             e = lg_c<T>::table(_n).end(),
             t = lg_t<T>::table(_n).begin();
        T sum(0);
        for( ; c != e; ++c, ++t)
            sum += *c * lgq(*t, pts...);
        return sum / 2;
    }
}}

namespace niji { namespace bezier
{
    template<class T>
    std::array<point<T>, 17> const& quad_circle_points()
    {
        using constants::tan_pi_over_8;
        using constants::root2_over_2;

        static std::array<point<T>, 17> pts =
        {{
            point<T>(1, 0)
          , point<T>(1, tan_pi_over_8<T>())
          , point<T>(root2_over_2<T>(), root2_over_2<T>())
          , point<T>(tan_pi_over_8<T>(), 1)
        
          , point<T>(0, 1)
          , point<T>(-tan_pi_over_8<T>(), 1)
          , point<T>(-root2_over_2<T>(), root2_over_2<T>())
          , point<T>(-1, tan_pi_over_8<T>())
        
          , point<T>(-1, 0)
          , point<T>(-1, -tan_pi_over_8<T>())
          , point<T>(-root2_over_2<T>(), -root2_over_2<T>())
          , point<T>(-tan_pi_over_8<T>(), -1)
        
          , point<T>(0, -1)
          , point<T>(tan_pi_over_8<T>(), -1)
          , point<T>(root2_over_2<T>(), -root2_over_2<T>())
          , point<T>(1, -tan_pi_over_8<T>())
        
          , point<T>(1, 0)
        }};
        return pts;
    }
    
    template<class T>
    std::array<point<T>, 13> const& cubic_circle_points()
    {
        using constants::cubic_arc_factor;

        static std::array<point<T>, 13> pts =
        {{
            point<T>(1, 0)
          , point<T>(1, cubic_arc_factor<T>())
          , point<T>(cubic_arc_factor<T>(), 1)
          
          , point<T>(0, 1)
          , point<T>(-cubic_arc_factor<T>(), 1)
          , point<T>(-1, cubic_arc_factor<T>())
          
          , point<T>(-1, 0)
          , point<T>(-1, -cubic_arc_factor<T>())
          , point<T>(-cubic_arc_factor<T>(), -1)
          
          , point<T>(0, -1)
          , point<T>(cubic_arc_factor<T>(), -1)
          , point<T>(1, -cubic_arc_factor<T>())
          
          , point<T>(1, 0)
        }};
        return pts;
    }
    
    template<class T>
    inline T quad_length(point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
    {
        return detail::length_impl<T>(pt1, pt2, pt3);
    }

    template<class T>
    inline T cubic_length(point<T> const& pt1, point<T> const& pt2, point<T> const& pt3, point<T> const& pt4)
    {
        return detail::length_impl<T>(pt1, pt2, pt3, pt4);
    }

    // Find t value for quadratic [a, b, c] = d.
    // Return 0 if there is no solution within [0, 1)
    template<class T>
    T* quad_solve(T a, T b, T c, T d, T tValues[2])
    {
        // At^2 + Bt + C = d
        T A = a - 2 * b + c;
        T B = 2 * (b - a);
        T C = a - d;
        return detail::find_unit_quad_roots(A, B, C, tValues);
    }
    
    template<class T>
    inline T quad_eval(T a, T b, T c, T t)
    {
        BOOST_ASSERT(0 <= t && t <= 1);

        return interpolate(interpolate(a, b, t), interpolate(b, c, t), t);
    }
    
    template<class T>
    void chop_quad_at_half(point<T> const in[3], point<T> out[5])
    {
        out[0] = in[0];
        out[1] = points::middle(in[0], in[1]);
        out[3] = points::middle(in[1], in[2]);
        out[2] = points::middle(out[1], out[3]);
        out[4] = in[2];
    }

    template<class T>
    void chop_quad_at(point<T> const in[3], point<T> out[5], T t)
    {
        BOOST_ASSERT(0 <= t && t <= 1);
        
        out[0] = in[0];
        out[1] = points::interpolate(in[0], in[1], t);
        out[3] = points::interpolate(in[1], in[2], t);
        out[2] = points::interpolate(out[1], out[3], t);
        out[4] = in[2];
    }

    // F(t)    = a (1 - t) ^ 2 + 2 b t (1 - t) + c t ^ 2
    // F'(t)   = 2 (b - a) + 2 (a - 2b + c) t
    // F''(t)  = 2 (a - 2b + c)
    //
    // A = 2 (b - a)
    // B = 2 (a - 2b + c)
    //
    // Maximum curvature for a quadratic means solving
    // Fx' Fx'' + Fy' Fy'' = 0
    //
    // t = - (Ax Bx + Ay By) / (Bx ^ 2 + By ^ 2)
    template<class T>
    bool chop_quad_at_max_curvature(point<T> const in[3], point<T> out[5])
    {
        vector<T> A(in[1] - in[0]);
        vector<T> B(in[0] - (in[1] + in[1]) + in[2]);
        T numer = -vectors::dot(A, B), denom = vectors::norm_square(B);
        T t = (denom != 0 && numer > 0 && numer < denom)? (numer / denom) : 0;
        if (t)
        {
            chop_quad_at(in, out, t);
            return true;
        }
        // no chop
        return false;
    }
    
    // given a quad-curve and a point (x,y), chop the quad at that point and place
    // the new off-curve point and endpoint into 'dest'.
    // Should only return false if the computed pos is the start of the curve
    // (i.e. root == 0)
    template<class T>
    bool truncate_quad_at(point<T> quad[3], point<T> const& pt)
    {
        using std::abs;
        
        T const* base;
        T value, roots[2];
    
        if (abs(pt.x) < abs(pt.y))
        {
            base = &quad[0].x;
            value = pt.x;
        }
        else
        {
            base = &quad[0].y;
            value = pt.y;
        }
    
        // note: this returns 0 if it thinks value is out of range, meaning the
        // root might return something outside of [0, 1)
        auto end = quad_solve(base[0], base[2], base[4], value, roots);
        if (roots != end && *roots > 0)
        {
            quad[1] = points::interpolate(quad[0], quad[1], *roots);
            quad[2] = pt;
            return true;
        }
        else
        {
            // t == 0 means either the value triggered a root outside of [0, 1)
            // For our purposes, we can ignore the <= 0 roots, but we want to
            // catch the >= 1 roots (which given our caller, will basically mean
            // a root of 1, give-or-take numerical instability). If we are in the
            // >= 1 case, return the existing offCurve point.
            //    
            // The test below checks to see if we are close to the "end" of the
            // curve (near base[4]). Rather than specifying a tolerance, I just
            // check to see if value is on to the right/left of the middle point
            // (depending on the direction/sign of the end points).
            if ((base[0] < base[4] && value > base[2]) ||
                (base[0] > base[4] && value < base[2]))   // should root have been 1
            {
                quad[2] = pt;
                return true;
            }
        }
        return false;
    }
    
    template<class T>
    point<T>* build_quad_arc(vector<T> const& u_start, vector<T> const& u_stop, bool is_ccw, transforms::affine<T> const* mat, point<T>* out)
    {
        using std::abs;

        T x = vectors::dot(u_start, u_stop);
        T y = vectors::cross(u_start, u_stop);
        
        T abs_x = abs(x), abs_y = abs(y);
        
        int point_count;
    
        // check for (effectively) coincident vectors
        // this can happen if our angle is nearly 0 or nearly 180 (y == 0)
        // ... we use the dot-prod to distinguish between 0 and 180 (x > 0)
        if (is_nearly_zero(abs_y) && x > 0 && ((y >= 0 && is_ccw) || (y <= 0 && !is_ccw)))
        {
            // just return the start-point
            out[0] = point<T>(1, 0);
            point_count = 1;
        }
        else
        {
            if (!is_ccw)
                y = -y;
    
            // what octant (quadratic curve) is [xy] in?
            int oct = 0;
            bool same_sign = true;
    
            if (0 == y)
            {
                oct = 4;        // 180
                BOOST_ASSERT(is_nearly_zero(abs(x + 1)));
            }
            else if (0 == x)
            {
                BOOST_ASSERT(is_nearly_zero(abs_y - 1));
                if (y > 0)
                    oct = 2;    // 90
                else
                    oct = 6;    // 270
            }
            else
            {
                if (y < 0)
                    oct += 4;
                if ((x < 0) != (y < 0))
                {
                    oct += 2;
                    same_sign = false;
                }
                if ((abs_x < abs_y) == same_sign)
                    oct += 1;
            }
    
            int whole_count = oct << 1;
            point<T> const* q = quad_circle_points<T>().begin();
            std::copy(q, q + (whole_count + 3), out);
            if (truncate_quad_at(out + whole_count, point<T>(x, y)))
                whole_count += 2;

            point_count = whole_count + 1;
        }
        transforms::affine<T> affine(transforms::rotate<T>(u_start.y, u_start.x));
        if (!is_ccw)
            affine.pre_flip_y();
        if (mat)
            affine.append(*mat);
        auto it = out, end = out + point_count;
        for ( ; it != end; ++it)
            *it = affine(*it);
        return it;
    }

    // Find t value for cubic [a, b, c, d] = e.
    template<class T>
    T* cubic_solve(T a, T b, T c, T d, T e, T tValues[3])
    {
        T C = 3 * (b - a);
        T B = 3 * (c - b) - C;
        T A = d - (a + C + B);
        return detail::solve_cubic_poly(A, B, C, a - e, tValues);
    }

    template<class T>
    T cubic_eval(T a, T b, T c, T d, T t)
    {
        BOOST_ASSERT(0 <= t && t <= 1);

        T ab = interpolate(a, b, t);
        T bc = interpolate(b, c, t);
        T cd = interpolate(c, d, t);
        T abc = interpolate(ab, bc, t);
        T bcd = interpolate(bc, cd, t);
        return interpolate(abc, bcd, t);
    }

    template<class T>
    void chop_cubic_at_half(point<T> const in[4], point<T> out[7])
    {
        point<T> bc = points::middle(in[1], in[2]);
    
        out[0] = in[0];
        out[1] = points::middle(in[0], in[1]);
        out[2] = points::middle(out[1], bc);
        out[5] = points::middle(in[2], in[3]);
        out[4] = points::middle(bc, out[5]);
        out[3] = points::middle(out[2], out[4]);
        out[6] = in[3];
    }
    
    template<class T>
    void chop_cubic_at(point<T> const in[4], point<T> out[7], T t)
    {
        BOOST_ASSERT(0 <= t && t <= 1);
        
        point<T> bc = points::interpolate(in[1], in[2], t);
    
        out[0] = in[0];
        out[1] = points::interpolate(in[0], in[1], t);
        out[2] = points::interpolate(out[1], bc, t);
        out[5] = points::interpolate(in[2], in[3], t);
        out[4] = points::interpolate(bc, out[5], t);
        out[3] = points::interpolate(out[2], out[4], t);
        out[6] = in[3];
    }

    template<class T>
    void chop_cubic_at(point<T> const in[4], point<T> out[], T* tit, T* tend)
    {
        if (!out)
            return;
            
        if (tit == tend)
        {
            std::copy(in, in + 4, out);
            return;
        }
        
        point<T> tmp[4];
        T t = *tit;
        T* end = tend - 1;
        for ( ; tit != end; ++tit)
        {
            chop_cubic_at(in, out, t);
            out += 3;
            // have src point to the remaining cubic (after the chop)
            std::copy(out, out + 4, tmp);
            in = tmp;
            
            // watch out in case the renormalized t isn't in range
            if (!valid_unit_divide(tit[1] - *tit, 1 - *tit, t))
            {
                // if we can't, just create a degenerate cubic
                out[4] = out[5] = out[6] = in[3];
                break;
            }
        }
        chop_cubic_at(in, out, t);
    }
    
    template<class T>
    std::ptrdiff_t chop_cubic_at_max_curvature(point<T> const in[4], point<T> out[13], T tValues[3])
    {
        auto it = detail::find_cubic_max_curvature(in, tValues);
        if (it == tValues)
            std::copy(in, in + 4, out);
        else
            chop_cubic_at(in, out, tValues, it);
        return it - tValues + 1;
    }

    // given a cubic-curve and a point (x,y), chop the cubic at that point and place
    // the new off-curve point and endpoint into 'dest'.
    // Should only return false if the computed pos is the start of the curve
    // (i.e. root == 0)
    template<class T>
    bool truncate_cubic_at(point<T> cubic[4], point<T> const& pt)
    {
        using std::abs;
        
        T const* base;
        T value, roots[3];
    
        if (abs(pt.x) < abs(pt.y))
        {
            base = &cubic[0].x;
            value = pt.x;
        }
        else
        {
            base = &cubic[0].y;
            value = pt.y;
        }
    
        // note: this returns 0 if it thinks value is out of range, meaning the
        // root might return something outside of [0, 1)
        auto end = cubic_solve(base[0], base[2], base[4], base[6], value, roots);
        if (roots != end && *roots > 0)
        {
            auto t = *roots;
            auto c1 = cubic[1];
            cubic[1] = points::interpolate(cubic[0], c1, t);
            cubic[2] = points::interpolate(cubic[1], points::interpolate(c1, cubic[2], t), t);
            cubic[3] = pt;
            return true;
        }
        else
        {
            // t == 0 means either the value triggered a root outside of [0, 1)
            // For our purposes, we can ignore the <= 0 roots, but we want to
            // catch the >= 1 roots (which given our caller, will basically mean
            // a root of 1, give-or-take numerical instability). If we are in the
            // >= 1 case, return the existing offCurve point.
            //    
            // The test below checks to see if we are close to the "end" of the
            // curve (near base[4]). Rather than specifying a tolerance, I just
            // check to see if value is on to the right/left of the middle point
            // (depending on the direction/sign of the end points).
            if ((base[0] < base[6]) == (value > base[4]))   // should root have been 1
            {
                cubic[3] = pt;
                return true;
            }
        }
        return false;
    }
    
    template<class T>
    point<T>* build_cubic_arc(vector<T> const& u_start, vector<T> const& u_stop, bool is_ccw, transforms::affine<T> const* mat, point<T>* out)
    {
        using std::abs;

        T x = vectors::dot(u_start, u_stop);
        T y = vectors::cross(u_start, u_stop);
        
        T abs_y = abs(y);
        
        int point_count;
    
        // check for (effectively) coincident vectors
        // this can happen if our angle is nearly 0 or nearly 180 (y == 0)
        // ... we use the dot-prod to distinguish between 0 and 180 (x > 0)
        if (is_nearly_zero(abs_y) && x > 0 && ((y >= 0 && is_ccw) || (y <= 0 && !is_ccw)))
        {
            // just return the start-point
            out[0] = point<T>(1, 0);
            point_count = 1;
        }
        else
        {
            if (!is_ccw)
                y = -y;
    
            // what quadrant (cubic curve) is [xy] in?
            int quad = 0;

            if (0 == y)
            {
                quad = 2;        // 180
                BOOST_ASSERT(is_nearly_zero(abs(x + 1)));
            }
            else if (0 == x)
            {
                BOOST_ASSERT(is_nearly_zero(abs_y - 1));
                if (y > 0)
                    quad = 1;    // 90
                else
                    quad = 3;    // 270
            }
            else
            {
                if (y < 0)
                    quad += 2;
                if ((x < 0) != (y < 0))
                {
                    quad += 1;
                }
            }
    
            int whole_count = quad * 3;
            auto q = cubic_circle_points<T>().begin();
            std::copy(q, q + (whole_count + 4), out);
            if (truncate_cubic_at(out + whole_count, point<T>(x, y)))
                whole_count += 3;

            point_count = whole_count + 1;
        }
        transforms::affine<T> affine(transforms::rotate<T>(u_start.y, u_start.x));
        if (!is_ccw)
            affine.pre_flip_y();
        if (mat)
            affine.append(*mat);
        auto it = out, end = out + point_count;
        for ( ; it != end; ++it)
            *it = affine(*it);
        return it;
    }
}}

#endif