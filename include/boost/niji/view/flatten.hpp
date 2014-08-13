/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_VIEW_FLATTEN_HPP_INCLUDED
#define BOOST_NIJI_VIEW_FLATTEN_HPP_INCLUDED

#include <boost/niji/support/traits.hpp>
#include <boost/niji/support/view.hpp>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/bezier/flatten.hpp>

namespace boost { namespace niji
{
    template<class Policy = bezier::flatten_policy<double>>
    struct flatten_view : view<flatten_view<Policy>>
    {
        using T = typename Policy::value_type;
        using point_t = point<T>;
        
        template<class Path>
        using point_type = point_t;

        template<class Sink>
        struct adaptor
        {
            template<class Tag>
            void operator()(Tag tag, point_t const& pt)
            {
                _sink(tag, pt);
                _pt = pt;
            }

            void operator()(quad_to_t, point_t const& pt1, point_t const& pt2)
            {
                _flatten(_pt, pt1, pt2, liner());
                _pt = pt2;
            }

            void operator()(cubic_to_t, point_t const& pt1, point_t const& pt2, point_t const& pt3)
            {
                _flatten(_pt, pt1, pt2, pt3, liner());
                _pt = pt3;
            }
    
            template<class Tag>
            void operator()(Tag tag)
            {
                _sink(tag);
            }

            auto liner() const
            {
                return [&sink=_sink](point_t const& pt)
                {
                    sink(command::line_to, pt);
                };
            }

            Sink& _sink;
            bezier::flattener<Policy> const& _flatten;
            point_t _pt;
        };
        
        explicit flatten_view(T angle_tolerance = 0, T approx_scale = 1, T cusp_limit = 0)
          : _flatten(angle_tolerance, approx_scale, cusp_limit)
        {}

        template<class Path, class Sink>
        void iterate(Path const& path, Sink& sink) const
        {
             niji::iterate(path, adaptor<Sink>{sink, _flatten});
        }
                
        template<class Path, class Sink>
        void reverse_iterate(Path const& path, Sink& sink) const
        {
             niji::reverse_iterate(path, adaptor<Sink>{sink, _flatten});
        }
        
    private:
        
        bezier::flattener<Policy> _flatten;
    };
    
    template<class Path, class Policy>
    struct path_cache<path_view<Path, flatten_view<Policy>>>
      : default_path_cache<path_view<Path, flatten_view<Policy>>>
    {};
}}

#endif
