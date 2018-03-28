/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2018 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_VIEW_DETAIL_STROKER_HPP_INCLUDED
#define NIJI_VIEW_DETAIL_STROKER_HPP_INCLUDED

#include <niji/view/detail/offset_outline.hpp>

namespace niji { namespace detail
{
    template<class T, class Joiner, class Capper>
    struct stroker : offset_outline<T, Joiner, true>
    {
        using base = offset_outline<T, Joiner, true>;

        Capper const& _cap;

        stroker(T r, Joiner const& join, Capper const& cap)
          : base(r, join), _cap(cap)
        {}

        void degenerated_dot()
        {
            _cap(base::_outer, base::_prev_pt, vector_t(base::_r, 0), true);
            _cap(base::_outer, base::_prev_pt, vector_t(-base::_r, 0), true);
        }

        void move_to(point_t const& pt)
        {
            cut(false);
            base::move_to_no_cap(pt);
        }

        void cut(bool curr_is_line)
        {
            switch (base::_seg_count)
            {
            case -1:
                degenerated_dot();
            case 0:
                return;
            }
            // cap the end
            _cap(base::_outer, base::_prev_pt, base::_prev_normal, curr_is_line);
            base::_outer.reverse_splice(base::_inner);

            // cap the start
            _cap(base::_outer, base::_first_pt, -base::_first_normal, base::_prev_is_line);
            base::_outer.close();

            base::_inner.clear();
        }

        void close(bool curr_is_line)
        {
            if (base::_seg_count < 1)
            {
                if (base::_seg_count == -1)
                    degenerated_dot();
                return;
            }
            base::close(curr_is_line);
        }
    };
}}

#endif