/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_BEZIER_FLATTEN_HPP_INCLUDED
#define BOOST_NIJI_BEZIER_FLATTEN_HPP_INCLUDED

#include <type_traits>
#include <boost/math/special_functions/pow.hpp>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/point.hpp>
#include <boost/niji/support/math/constants.hpp>
#include <boost/niji/support/math/functions.hpp>
#include <boost/niji/support/utility/enable_if.hpp>

namespace boost { namespace niji { namespace bezier
{
    template<class T, class Enable = void>
    struct flatten_policy;
    
    template<class T>
    struct flatten_policy<T, enable_if_t<std::is_floating_point<T>>>
    {
        using value_type = T;

        static inline bool no_angle_tolerance(T val)
        {
            return val < 0.01;
        }

        static inline bool not_collinear(T val)
        {
            return val > 1e-30;
        }
        
        static constexpr unsigned recursion_limit = 32;
    };

    template<class Policy>
    struct flattener
    {
        using T = typename Policy::value_type;
        using point_t = point<T>;

        explicit flattener(T angle_tolerance = 0, T approx_scale = 1, T cusp_limit = 0)
          : _distance_tolerance_square(math::pow<2>(T(0.5) / approx_scale))
          , _angle_tolerance(Policy::no_angle_tolerance(angle_tolerance)? 0 : angle_tolerance)
          , _cusp_limit(cusp_limit)
        {}
        
        template<class LineTo>
        void operator()(point_t const& pt1, point_t const& pt2, point_t const& pt3, LineTo&& line_to) const
        {
            quad(line_to, pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, 0);
            line_to(pt3);
        }
        
        template<class LineTo>
        void operator()(point_t const& pt1, point_t const& pt2, point_t const& pt3, point_t const& pt4, LineTo&& line_to) const
        {
            cubic(line_to, pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, pt4.x, pt4.y, 0);
            line_to(pt4);
        }
        
    private:

        template<class LineTo>
        void quad(LineTo& line_to, T x1, T y1, T x2, T y2, T x3, T y3, unsigned level) const
        {
            using std::abs;
            using std::atan2;
            
            if (level > Policy::recursion_limit)
                return;

            // Calculate all the mid-points of the line segments
            //----------------------
            T x12   = (x1 + x2) / 2;
            T y12   = (y1 + y2) / 2;
            T x23   = (x2 + x3) / 2;
            T y23   = (y2 + y3) / 2;
            T x123  = (x12 + x23) / 2;
            T y123  = (y12 + y23) / 2;

            T dx = x3 - x1;
            T dy = y3 - y1;
            T d = abs((x2 - x3) * dy - (y2 - y3) * dx);
            T da;

            if (Policy::not_collinear(d))
            {
                // Regular case
                //-----------------
                if (d * d <= _distance_tolerance_square * square_sum(dx, dy))
                {
                    // If the curvature doesn't exceed the distance_tolerance value
                    // we tend to finish subdivisions.
                    //----------------------
                    if (!_angle_tolerance)
                    {
                        line_to(point_t(x123, y123));
                        return;
                    }

                    // Angle & Cusp Condition
                    //----------------------
                    da = abs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1));
                    if (da >= math::constants::pi<T>())
                        da = 2 * math::constants::pi<T>() - da;
                    if (da < _angle_tolerance)
                    {
                        // Finally we can stop the recursion
                        //----------------------
                        line_to(point_t(x123, y123));
                        return;
                    }
                }
            }
            else
            {
                // Collinear case
                //------------------
                da = square_sum(dx, dy);
                if (da == 0)
                {
                    d = square_sum(x1 - x2, y1 - y2);
                }
                else
                {
                    d = ((x2 - x1) * dx + (y2 - y1) * dy) / da;
                    // Simple collinear case, 1---2---3
                    // We can leave just two endpoints
                    if (0 < d && d < 1)
                        return;
                    if (d <= 0)
                        d = square_sum(x1 - x2, y1 - y2);
                    else if (d >= 1)
                        d = square_sum(x3 - x2, y3 - y2);
                    else
                        d = square_sum(x1 - x2 + d * dx, y1 - y2 + d * dy);
                }
                if (d < _distance_tolerance_square)
                {
                    line_to(point_t(x2, y2));
                    return;
                }
            }

            // Continue subdivision
            //----------------------
            quad(line_to, x1, y1, x12, y12, x123, y123, level + 1);
            quad(line_to, x123, y123, x23, y23, x3, y3, level + 1);
        }
        
        template<class LineTo>
        void cubic(LineTo& line_to, T x1, T y1, T x2, T y2, T x3, T y3, T x4, T y4, unsigned level) const
        {
            using std::abs;
            using std::atan2;

            if (level > Policy::recursion_limit)
                return;

            // Calculate all the mid-points of the line segments
            //----------------------
            T x12   = (x1 + x2) / 2;
            T y12   = (y1 + y2) / 2;
            T x23   = (x2 + x3) / 2;
            T y23   = (y2 + y3) / 2;
            T x34   = (x3 + x4) / 2;
            T y34   = (y3 + y4) / 2;
            T x123  = (x12 + x23) / 2;
            T y123  = (y12 + y23) / 2;
            T x234  = (x23 + x34) / 2;
            T y234  = (y23 + y34) / 2;
            T x1234 = (x123 + x234) / 2;
            T y1234 = (y123 + y234) / 2;


            // Try to approximate the full cubic curve by a single straight line
            //------------------
            T dx = x4 - x1;
            T dy = y4 - y1;

            T d2 = abs(((x2 - x4) * dy - (y2 - y4) * dx));
            T d3 = abs(((x3 - x4) * dy - (y3 - y4) * dx));
            T da1, da2, k;

            switch (Policy::not_collinear(d2) << 1 | int(Policy::not_collinear(d3)))
            {
            case 0:
                // All collinear OR p1==p4
                //----------------------
                k = square_sum(dx, dy);
                if (k == 0)
                {
                    d2 = square_sum(x2 - x1, y2 - y1);
                    d3 = square_sum(x3 - x4, y3 - y4);
                }
                else
                {
                    k   = 1 / k;
                    da1 = x2 - x1;
                    da2 = y2 - y1;
                    d2  = k * (da1 * dx + da2 * dy);
                    da1 = x3 - x1;
                    da2 = y3 - y1;
                    d3  = k * (da1 * dx + da2 * dy);
                    // Simple collinear case, 1---2---3---4
                    // We can leave just two endpoints
                    if (d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1)
                        return;
                    if (d2 <= 0)
                        d2 = square_sum(x1 - x2, y1 - y2);
                    else if (d2 >= 1)
                        d2 = square_sum(x4 - x2, y4 - y2);
                    else
                        d2 = square_sum(x1 + d2 * dx - x2, y1 + d2 * dy - y2);

                    if (d3 <= 0)
                        d3 = square_sum(x1 - x3, y1 - y3);
                    else if (d3 >= 1)
                        d3 = square_sum(x4 - x3, y4 - y3);
                    else
                        d3 = square_sum(x1 + d3 * dx - x3, y1 + d3 * dy - y3);
                }
                if (d2 > d3)
                {
                    if (d2 < _distance_tolerance_square)
                    {
                        line_to(point_t(x2, y2));
                        return;
                    }
                }
                else
                {
                    if (d3 < _distance_tolerance_square)
                    {
                        line_to(point_t(x3, y3));
                        return;
                    }
                }
                break;

            case 1:
                // p1,p2,p4 are collinear, p3 is significant
                //----------------------
                if (d3 * d3 <= _distance_tolerance_square * (square_sum(dx, dy)))
                {
                    if (!_angle_tolerance)
                    {
                        line_to(point_t(x23, y23));
                        return;
                    }

                    // Angle Condition
                    //----------------------
                    da1 = abs(atan2(y4 - y3, x4 - x3) - atan2(y3 - y2, x3 - x2));
                    if (da1 >= math::constants::pi<T>())
                        da1 = 2 * math::constants::pi<T>() - da1;
                    if (da1 < _angle_tolerance)
                    {
                        line_to(point_t(x2, y2));
                        line_to(point_t(x3, y3));
                        return;
                    }
                    if (_cusp_limit != 0)
                    {
                        if (da1 > _cusp_limit)
                        {
                            line_to(point_t(x3, y3));
                            return;
                        }
                    }
                }
                break;

            case 2:
                // p1,p3,p4 are collinear, p2 is significant
                //----------------------
                if (d2 * d2 <= _distance_tolerance_square * (square_sum(dx, dy)))
                {
                    if (!_angle_tolerance)
                    {
                        line_to(point_t(x23, y23));
                        return;
                    }

                    // Angle Condition
                    //----------------------
                    da1 = abs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1));
                    if (da1 >= math::constants::pi<T>())
                        da1 = 2 * math::constants::pi<T>() - da1;
                    if (da1 < _angle_tolerance)
                    {
                        line_to(point_t(x2, y2));
                        line_to(point_t(x3, y3));
                        return;
                    }
                    if (_cusp_limit != 0)
                    {
                        if (da1 > _cusp_limit)
                        {
                            line_to(point_t(x2, y2));
                            return;
                        }
                    }
                }
                break;

            case 3:
                // Regular case
                //-----------------
                if ((d2 + d3)*(d2 + d3) <= _distance_tolerance_square * (square_sum(dx, dy)))
                {
                    // If the curvature doesn't exceed the distance_tolerance value
                    // we tend to finish subdivisions.
                    //----------------------
                    if (!_angle_tolerance)
                    {
                        line_to(point_t(x23, y23));
                        return;
                    }

                    // Angle & Cusp Condition
                    //----------------------
                    k   = atan2(y3 - y2, x3 - x2);
                    da1 = abs(k - atan2(y2 - y1, x2 - x1));
                    da2 = abs(atan2(y4 - y3, x4 - x3) - k);
                    if (da1 >= math::constants::pi<T>())
                        da1 = 2 * math::constants::pi<T>() - da1;
                    if (da2 >= math::constants::pi<T>())
                        da2 = 2 * math::constants::pi<T>() - da2;
                    if (da1 + da2 < _angle_tolerance)
                    {
                        // Finally we can stop the recursion
                        //----------------------
                        line_to(point_t(x23, y23));
                        return;
                    }
                    if (_cusp_limit != 0)
                    {
                        if (da1 > _cusp_limit)
                        {
                            line_to(point_t(x2, y2));
                            return;
                        }
                        if (da2 > _cusp_limit)
                        {
                            line_to(point_t(x3, y3));
                            return;
                        }
                    }
                }
                break;
            }

            // Continue subdivision
            //----------------------
            cubic(line_to, x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1);
            cubic(line_to, x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1);
        }

        T _distance_tolerance_square, _angle_tolerance, _cusp_limit;
    };
}}}

#endif
