/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GRAPHIC_RADIAL_HPP_INCLUDED
#define NIJI_GRAPHIC_RADIAL_HPP_INCLUDED

#include <type_traits>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/single_view.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/pop_front.hpp>
#include <boost/fusion/include/zip.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <niji/support/command.hpp>
#include <niji/support/traits.hpp>
#include <niji/support/point.hpp>
#include <niji/support/math/constants.hpp>

namespace niji
{
    template<class T, class U = T>
    struct radial
    {
        using point_type = point<T>;
        
        using view_t = std::conditional_t<
            boost::fusion::traits::is_sequence<U>::value
          , U const&, boost::fusion::single_view<U>>;
        
        point_type origin;
        T r;
        U theta; // To turn side up, use radian = PI / n
        std::size_t n;

        radial(std::size_t n, point_type const& pt, U const& r, U const& theta = {})
          : origin(pt), r(r), theta(theta), n(n)
        {}

        template<class Sink>
        void render(Sink& sink) const
        {
            render_impl(sink, constants::two_pi<T>(), view_t(r), view_t(theta));
        }
        
        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            render_impl(sink, -constants::two_pi<T>(), view_t(r), view_t(theta));
        }

    private:

        template<class Sink, class Us>
        void render_impl(Sink& sink, T da, Us const& r, Us const& theta) const
        {
            using namespace command;
            using std::sin;
            using std::cos;

            if (n < 3)
                return;
            
            da /= n;
            
            auto add_half_pi =
                [](T val) { return val + constants::half_pi<T>(); };

            typename boost::fusion::result_of::as_vector<Us>::type
                d(boost::fusion::transform(theta, add_half_pi));

            auto d1 = boost::fusion::at_c<0>(d);
            auto r1 = boost::fusion::at_c<0>(r);
            sink(move_to, point_type{origin.x + cos(d1) * r1, origin.y + sin(d1) * r1});
            boost::fusion::for_each(boost::fusion::pop_front(boost::fusion::zip(d, r)), [&sink, this](auto&& zip)
            {
                auto d = boost::fusion::at_c<0>(zip);
                auto r = boost::fusion::at_c<1>(zip);
                sink(line_to, point_type{origin.x + cos(d) * r, origin.y + sin(d) * r});
            });
            auto fn = [&sink, da, this](T d, T r)
            {
                d += da;
                sink(line_to, point_type{origin.x + cos(d) * r, origin.y + sin(d) * r});
                return d;
            };
            std::size_t m = n - 1, k = (m - 1) >> 2, i = 0;
            switch (m & 3u)
            {
                do
                {
            case 0:
                    d = boost::fusion::transform(d, r, fn);
            case 3:
                    d = boost::fusion::transform(d, r, fn);
            case 2:
                    d = boost::fusion::transform(d, r, fn);
            case 1:
                    d = boost::fusion::transform(d, r, fn);
                } while (i++ != k);
            }
            sink(end_poly);
        }
    };
}

#endif