/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_BEZIER_BASICS_HPP_INCLUDED
#define BOOST_NIJI_BEZIER_BASICS_HPP_INCLUDED

#include <boost/assert.hpp>
#include <boost/array.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/vector.hpp>
#include <boost/niji/support/point.hpp>
#include <boost/niji/support/transform/affine.hpp>
#include <boost/niji/support/math/constants.hpp>
#include <boost/niji/support/math/functions.hpp>
#include <boost/niji/support/utility/enable_if.hpp>
#include <boost/niji/support/utility/pack.hpp>

namespace boost { namespace niji { namespace detail
{
    // Q = -1/2 (B + sign(B) sqrt[B*B - 4*A*C])
    // x1 = Q / A
    // x2 = C / Q
    //
    template<class T>
    int find_unit_quad_roots(T a, T b, T c, T roots[2])
    {
        using std::sqrt;
        using std::swap;
    
        if (a == 0)
            return valid_unit_divide(-c, b, *roots);
    
        T* r = roots;

        T root = b * b - 4 * a * c;
        if (root < 0 || math::isnan(root)) // complex roots
            return 0;

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
        return r - roots;
    }
    
    template<class T>
    T quad_solve(T a, T b, T c, T d)
    {
        // At^2 + Bt + C = d
        T A = a - 2 * b + c;
        T B = 2 * (b - a);
        T C = a - d;
    
        T    roots[2];
        int  count = find_unit_quad_roots(A, B, C, roots);
    
        BOOST_ASSERT(count <= 1);
        return count == 1 ? roots[0] : 0;
    }
    
    template<class T>
    T find_quad_extrema(T a, T b, T c)
    {
        using std::abs;
        
        // Quad'(t) = At + B, where
        // A = 2(a - 2b + c)
        // B = 2(b - a)
        // Solve for t, only if it fits between 0 < t < 1
        T t;
        if (valid_unit_divide(a - b, a - b - b + c, t))
        {
            auto interp = [t](T a, T b) { return a + (b - a) * t; };
            return interp(interp(a, b), interp(b, c));
        }
        return abs(a - b) < abs(b - c) ? a : c;    
    }

    template<int n, int k, class = void>
    struct binomial_coeff;

    template<>
    struct binomial_coeff<0, 0>
    {
        static constexpr unsigned value = 1;
    };

    template<int n, int k>
    struct binomial_coeff<n, k, enable_if_c_t<(k <= n)>>
    {
        typedef typename
            mpl::if_c<!!k, binomial_coeff<n - 1, k - 1>, mpl::int_<0> >::type
        lhs;
    
        typedef typename
            mpl::if_c<(k > n - 1), mpl::int_<0>, binomial_coeff<n - 1, k> >::type
        rhs;
    
        static constexpr unsigned value = lhs::value + rhs::value;
    };

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
            return loop<T>(t)(mpl::int_<0>(), ts...);
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
            T operator()(mpl::int_<k>, T t0, Ts... ts)
            {
                T ret(binomial_coeff<m, k>::value * pow_t2[k] * pow_t * t0);
                pow_t *= t;
                return ret + (*this)(mpl::int_<k + 1>(), ts...);
            }

            T operator()(mpl::int_<n>)
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
        static array<T, 3> const& table(mpl::int_<3>)
        {
            static array<T, 3> const t =
            {{
                BOOST_NIJI_CONSTANTS
                (T,
                    (0.5)
                    (0.1127016653792583114820734600217600389168)
                    (0.8872983346207416885179265399782399610832)
                )
            }};
            return t;
        }

        static array<T, 4> const& table(mpl::int_<4>)
        {
            static array<T, 4> const t =
            {{
                BOOST_NIJI_CONSTANTS
                (T,
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
        static array<T, 3> const& table(mpl::int_<3>)
        {
            static array<T, 3> const t =
            {{
                BOOST_NIJI_CONSTANTS
                (T,
                    (0.8888888888888888888888888888888888888888)
                    (0.5555555555555555555555555555555555555555)
                    (0.5555555555555555555555555555555555555555)
                )
            }};
            return t;
        }

        static array<T, 4> const& table(mpl::int_<4>)
        {
            static array<T, 4> const t =
            {{
                BOOST_NIJI_CONSTANTS
                (T,
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
        mpl::int_<sizeof...(Points)> _n;
        T const *c = lg_c<T>::table(_n).begin(),
                *e = lg_c<T>::table(_n).end(),
                *t = lg_t<T>::table(_n).begin();
        T sum(0);
        for( ; c != e; ++c, ++t)
            sum += *c * lgq(*t, pts...);
        return sum / 2;
    }
}}}

namespace boost { namespace niji { namespace bezier
{
    template<class T>
    array<point<T>, 17> const& quad_circle_points()
    {
        using constants::tan_pi_over_8;
        using constants::root2_over_2;

        static array<point<T>, 17> pts =
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
    inline T length(point<T> const& pt1, point<T> const& pt2, point<T> const& pt3)
    {
        return detail::length_impl<T>(pt1, pt2, pt3);
    }

    template<class T>
    inline T length(point<T> const& pt1, point<T> const& pt2, point<T> const& pt3, point<T> const& pt4)
    {
        return detail::length_impl<T>(pt1, pt2, pt3, pt4);
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
        out[0] = in[0];
        out[1] = points::interpolate(in[0], in[1], t);
        out[3] = points::interpolate(in[1], in[2], t);
        out[2] = points::interpolate(out[1], out[3], t);
        out[4] = in[2];
    }
    
    template<class T>
    point<T> quad_at(point<T> const in[3], T t)
    {
        return points::interpolate
        (
            points::interpolate(in[0], in[1], t)
          , points::interpolate(in[1], in[2], t)
          , t
        );
    }

    //  F(t)    = a (1 - t) ^ 2 + 2 b t (1 - t) + c t ^ 2
    //  F'(t)   = 2 (b - a) + 2 (a - 2b + c) t
    //  F''(t)  = 2 (a - 2b + c)
    //
    //  A = 2 (b - a)
    //  B = 2 (a - 2b + c)
    //
    //  Maximum curvature for a quadratic means solving
    //  Fx' Fx'' + Fy' Fy'' = 0
    //
    //  t = - (Ax Bx + Ay By) / (Bx ^ 2 + By ^ 2)
    //  
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
    //
    template<class T>
    bool truncate_quad_at(point<T> quad[3], point<T> const& pt)
    {
        using std::abs;
        
        T const* base;
        T value;
    
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
        T t = detail::quad_solve(base[0], base[2], base[4], value);
    
        if (t > 0)
        {
            quad[1] = points::interpolate(quad[0], quad[1], t);
            quad[2] = pt;
            return true;
        }
        else
        {
            /*  t == 0 means either the value triggered a root outside of [0, 1)
                For our purposes, we can ignore the <= 0 roots, but we want to
                catch the >= 1 roots (which given our caller, will basically mean
                a root of 1, give-or-take numerical instability). If we are in the
                >= 1 case, return the existing offCurve point.
    
                The test below checks to see if we are close to the "end" of the
                curve (near base[4]). Rather than specifying a tolerance, I just
                check to see if value is on to the right/left of the middle point
                (depending on the direction/sign of the end points).
            */
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
        transforms::affine<T> affine;
        affine.set_sin_cos(u_start.y, u_start.x);
        if (!is_ccw)
            affine.pre_flip_y();
        if (mat)
            affine.append(*mat);
        point<T> *it = out, *end = out + point_count;
        for ( ; it != end; ++it)
            *it = affine(*it);
        return it;
    }
}}}

#endif
