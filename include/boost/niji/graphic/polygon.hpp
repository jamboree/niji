/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_GRAPHIC_POLYGON_HPP_INCLUDED
#define BOOST_NIJI_GRAPHIC_POLYGON_HPP_INCLUDED

#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/single_view.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/pop_front.hpp>
#include <boost/fusion/include/zip.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/math/constants.hpp>
#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/point.hpp>

namespace boost { namespace niji
{
    template<class T, class U = T>
    struct polygon
    {
        using point_type = point<T>;
        
        using view_t = typename
            mpl::if_<fusion::traits::is_sequence<U>,
                U const&, fusion::single_view<U>>::type;
        
        point_type origin;
        T r;
        T theta; // To turn side up, use radian = PI / n
        std::size_t n;

        polygon(std::size_t n, point_type const& pt, U const& r, U const& theta = {})
          : origin(pt), r(r), theta(theta), n(n)
        {}

        template<class Sink>
        void iterate(Sink& sink) const
        {
            iterate_impl(sink, constants::two_pi<T>(), view_t(r), view_t(theta));
        }
        
        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            iterate_impl(sink, -constants::two_pi<T>(), view_t(r), view_t(theta));
        }

    private:

        template<class Sink, class Us>
        void iterate_impl(Sink& sink, T da, Us const& r, Us const& theta) const
        {
            using namespace command;
            using std::sin;
            using std::cos;

            if (n < 3)
                return;
            
            da /= n;
            
            auto add_half_pi =
                [](T val) { return val + constants::half_pi<T>(); };

            typename fusion::result_of::as_vector<Us>::type
                d(fusion::transform(theta, add_half_pi));

            auto d1 = fusion::at_c<0>(d);
            auto r1 = fusion::at_c<0>(r);
            sink(move_to, point_type{origin.x + cos(d1) * r1, origin.y + sin(d1) * r1});
            fusion::for_each(fusion::pop_front(fusion::zip(d, r)), [&sink, this](auto&& zip)
            {
                auto d = fusion::at_c<0>(zip);
                auto r = fusion::at_c<1>(zip);
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
                    d = fusion::transform(d, r, fn);
            case 3:
                    d = fusion::transform(d, r, fn);
            case 2:
                    d = fusion::transform(d, r, fn);
            case 1:
                    d = fusion::transform(d, r, fn);
                } while (i++ != k);
            }
            sink(end_poly);
        }
    };
}}

#endif
