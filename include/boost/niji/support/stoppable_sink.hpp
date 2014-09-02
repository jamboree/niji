/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_STOPPABLE_SINK_HPP_INCLUDED
#define BOOST_NIJI_STOPPABLE_SINK_HPP_INCLUDED

namespace boost { namespace niji
{
    struct iteration_stopped {};
    
    class stoppable_sink
    {
    protected:
        
        static void stop()
        {
            throw iteration_stopped();
        }
    };
}}

#endif
