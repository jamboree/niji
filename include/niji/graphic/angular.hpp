/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2018 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_GRAPHIC_ANGULAR_HPP_INCLUDED
#define NIJI_GRAPHIC_ANGULAR_HPP_INCLUDED

#include <array>
#include <type_traits>
#include <niji/support/command.hpp>
#include <niji/support/traits.hpp>
#include <niji/support/point.hpp>
#include <niji/support/constants.hpp>

namespace niji
{
    template<class T, std::size_t N = 1>
    struct angular
    {
        using point_type = point<T>;

        using store_t = std::array<T, N>;
        using param_t = std::conditional_t<(N > 1), store_t const&, T>;

        point_type origin;
        store_t r;
        store_t theta; // To turn side up, use radian = PI / n
        unsigned n;

        angular(std::size_t n, point_type const& pt, param_t r, param_t theta = {})
          : origin(pt), r{r}, theta{theta}, n(n)
        {}

        template<class Sink>
        void render(Sink& sink) const
        {
            render_impl(sink, constants::two_pi<T>());
        }
        
        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            render_impl(sink, -constants::two_pi<T>());
        }

    private:

        template<class Sink>
        void render_impl(Sink& sink, T da) const
        {
            using namespace command;
            using std::sin;
            using std::cos;

            if (n < 3)
                return;
            
            da /= n;

            std::array<T, N> d(theta);
            for (T& di : d)
                di += constants::half_pi<T>();

            auto d1 = d[0];
            auto r1 = r[0];
            sink(move_to, point_type{origin.x + cos(d1) * r1, origin.y + sin(d1) * r1});
            for (std::size_t i = 0; i != N; ++i)
            {
                auto di = d[i];
                auto ri = r[i];
                sink(line_to, point_type{origin.x + cos(di) * ri, origin.y + sin(di) * ri});
            }

            auto fn = [&]
            {
                for (std::size_t i = 0; i != N; ++i)
                {
                    auto di = d[i] += da;
                    auto ri = r[i];
                    sink(line_to, point_type{origin.x + cos(di) * ri, origin.y + sin(di) * ri});
                }
            };

            unsigned m = n - 1, k = (m - 1) >> 2, i = 0;
            switch (m & 3u)
            {
                do
                {
            case 0: fn();
            case 3: fn();
            case 2: fn();
            case 1: fn();
                } while (i++ != k);
            }
            sink(end_closed);
        }
    };
}

#endif