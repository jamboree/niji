/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_STOPPABLE_SINK_HPP_INCLUDED
#define BOOST_NIJI_STOPPABLE_SINK_HPP_INCLUDED

namespace boost { namespace niji { namespace detail
{
    struct iteration_stopped {};
    struct stoppable_guard;
}}}

namespace boost { namespace niji
{
    struct stoppable_sink
    {
        void stop() const
        {
            throw detail::iteration_stopped();
        }
    
    private:
        
        friend struct detail::stoppable_guard;
        unsigned sink_depth = 0;
    };
}}

#endif
