/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_GROUP_HPP_INCLUDED
#define BOOST_NIJI_GROUP_HPP_INCLUDED

#include <boost/next_prior.hpp>
#include <boost/utility/swap.hpp>
#include <boost/container/deque.hpp>
#include <boost/niji/support/traits.hpp>

namespace boost { namespace niji
{
    template<class Path, class Alloc = std::allocator<Path> >
    class group : container::deque<Path, Alloc>
    {
        using base_type = container::deque<Path, Alloc>;
        
    public:
        
        using point_type = path_point_t<Path>;
        using path_type = Path;

        // Iterators
        //----------------------------------------------------------------------
        using iterator = typename base_type::iterator;
        using base_type::const_iterator;
        using base_type::begin;
        using base_type::end;

        // Observers
        //----------------------------------------------------------------------
        using base_type::front;
        using base_type::back;
        using base_type::size;
        using base_type::empty;
        using base_type::at;
        using base_type::operator[];        

        // Constructors
        //----------------------------------------------------------------------
        explicit group(Alloc const& alloc = Alloc()) noexcept
          : base_type(alloc)
        {}

        // Modifiers
        //----------------------------------------------------------------------
        using base_type::erase;
        using base_type::clear;
        
        iterator swap_erase(iterator const& pos)
        {
            if (boost::next(pos) == base_type::end())
            {
                base_type::pop_back();
                return base_type::end();
            }
            boost::swap(base_type::front(), *pos);
            base_type::pop_front();
            return pos;
        }
        
        void swap(group& other) noexcept
        {
            base_type::swap(other);
        }
        
        template<class T>
        void add(T&& path)
        {
            base_type::emplace_back(std::forward<T>(path));
        }

        // Traverse
        //----------------------------------------------------------------------
        template<class Sink>
        void iterate(Sink& sink) const
        {
            for (auto const& path : *this)
                niji::iterate(path, sink);
        };

        template<class Sink>
        void reverse_iterate(Sink& sink) const
        {
            for (auto const& path : *this)
                niji::reverse_iterate(path, sink);
        };

        template<class Archive>
        void serialize(Archive & ar, unsigned version)
        {
            ar & *static_cast<base_type*>(this);
        }
    };
}}

#endif
