/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_PATH_HPP_INCLUDED
#define NIJI_PATH_HPP_INCLUDED

#include <initializer_list>
#include <boost/assert.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/deque.hpp>
#include <boost/container/allocator_traits.hpp>
#include <niji/path_fwd.hpp>
#include <niji/render.hpp>
#include <niji/detail/path.hpp>

namespace niji
{
    template<class Node, class Alloc>
    class path : boost::container::deque<Node, Alloc>
    {
        template<class N, class A>
        friend class path;

        using nodes_base = boost::container::deque<Node, Alloc>;
        using index_tag_t = detail::index_tag_t;
        using index_tag_alloc_t =
            typename boost::container::allocator_traits<Alloc>::template
                portable_rebind_alloc<index_tag_t>::type;
        using index_tag_container =
            boost::container::vector<index_tag_t, index_tag_alloc_t>;
        using index_tag_iterator = typename index_tag_container::const_iterator;

    public:
        
        using point_type = Node;

        // Iterators
        //----------------------------------------------------------------------
        using iterator = typename nodes_base::iterator;
        using const_iterator = typename nodes_base::const_iterator;
        using nodes_base::begin;
        using nodes_base::end;

        // Observers
        //----------------------------------------------------------------------
        using nodes_base::front;
        using nodes_base::back;
        using nodes_base::size;
        using nodes_base::empty;

        using figures_view = detail::path_partition<iterator, index_tag_iterator>;
        using const_figures_view = detail::path_partition<const_iterator, index_tag_iterator>;
        using incomplete_view = detail::incomplete_path<const_iterator, index_tag_iterator>;

        figures_view figures()
        {
            return {nodes_base::begin(), nodes_base::end(), _index_tags.begin(), _index_tags.end()};
        }

        const_figures_view figures() const
        {
            return {nodes_base::begin(), nodes_base::end(), _index_tags.begin(), _index_tags.end()};
        }

        incomplete_view incomplete() const
        {
            return {nodes_base::begin(), nodes_base::end(), _index_tags.begin(), _index_tags.end()};
        }

        bool is_box() const
        {
            return detail::path_is_box(nodes_base::begin(), nodes_base::end());
        }

        bool is_ended() const
        {
            return nodes_base::empty() || 
                (!_index_tags.empty() &&
                    _index_tags.back().index == nodes_base::size());
        }

        struct sink
        {
            explicit sink(path& own, bool moving = true)
              : _own(own), _moving(moving)
            {}

            // silent MSVC warning C4512
            sink& operator=(sink const&) = delete;

            void operator()(move_to_t, Node const& pt)
            {
                _prev = pt;
                _moving = true;
            }

            void operator()(line_to_t, Node const& pt)
            {
                line_start();
                _own.join(pt);
            }

            void operator()(quad_to_t, Node const& pt1, Node const& pt2)
            {
                line_start();
                _own.unsafe_quad_to(pt1, pt2);
            }

            void operator()(cubic_to_t, Node const& pt1, Node const& pt2, Node const& pt3)
            {
                line_start();
                _own.unsafe_cubic_to(pt1, pt2, pt3);
            }

            void operator()(end_tag tag)
            {
                _own.delimit(tag);
                _moving = true;
            }
            
        private:
            
            void line_start()
            {
                if (_moving)
                {
                    _own.cut();
                    _own.join(_prev);
                    _moving = false;
                }
            }

            path& _own;
            Node _prev;
            bool _moving;
        };
        
        template<class Path>
        using requires_valid =
            std::enable_if_t<is_renderable<Path, sink>::value, bool>;

        // Constructors
        //----------------------------------------------------------------------
        path() = default;
        
        explicit path(Alloc const& alloc) noexcept
          : nodes_base(alloc), _index_tags(alloc)
        {}

        path(path const& other, Alloc const& alloc)
          : nodes_base(other, alloc)
          , _index_tags(other._index_tags, alloc)
        {}
                
        path(path&& other, Alloc const& alloc) noexcept
          : nodes_base(static_cast<nodes_base&&>(other), alloc)
          , _index_tags(std::move(other._index_tags), alloc)
        {}

        template<class Path, requires_valid<Path> = true>
        path(Path const& other, Alloc const& alloc = Alloc())
          : nodes_base(alloc), _index_tags(alloc)
        {
            add(other);
        }
        
        template<class Iter>
        path(Iter const& begin, Iter const& end, Alloc const& alloc = Alloc())
          : nodes_base(begin, end, alloc) , _index_tags(alloc)
        {}
        
        path(std::initializer_list<Node> pts, Alloc const& alloc = Alloc())
          : nodes_base(pts.begin(), pts.end(), alloc), _index_tags(alloc)
        {}

        template<class Path>
        path& operator=(Path const& other)
        {
            clear();
            add(other);
            return *this;
        }

        // Path Traversal
        //----------------------------------------------------------------------
        template<class Sink>
        void render(Sink& sink) const
        {
            if (detail::path_render_impl(sink, static_cast<nodes_base const&>(*this), _index_tags))
                sink(command::end_open);
        }
        
        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            namespace rng = ::boost::adaptors;
            using namespace command;

            char tag = end_tag::open;
            bool needs_ending = detail::path_render_impl
            (
                sink
              , rng::reverse(static_cast<nodes_base const&>(*this))
              , rng::transform(rng::reverse(_index_tags),
                    index_tag_t::remap(nodes_base::size(), tag))
            );
            if (needs_ending)
            {
                if (tag == end_tag::closed)
                    sink(end_closed);
                else
                    sink(end_open);
            }
        }

        // Modofiers
        //----------------------------------------------------------------------
        void join(Node const& v)
        {
            nodes_base::push_back(v);
        }
        
        template<class Iter>
        void join(Iter const& begin, Iter const& end)
        {
            nodes_base::insert(nodes_base::end(), begin, end);
        }

        void join_quad(Node const& pt1, Node const& pt2, Node const& pt3)
        {
            join(pt1);
            unsafe_quad_to(pt2, pt3);
        }

        void join_cubic(Node const& pt1, Node const& pt2, Node const& pt3, Node const& pt4)
        {
            join(pt1);
            unsafe_cubic_to(pt2, pt3, pt4);
        }

        template<class Nodes = std::initializer_list<Node>>
        void join_sequence(Nodes const& pts)
        {
            join(pts.begin(), pts.end());
        }

        // This is useful for paths that don't start with move_to, in which
        // case results in continuous path.
        template<class Path>
        void join_path(Path const& p)
        {
            niji::render(p, sink(*this, is_ended()));
        }

        // niji::path always starts with move_to.
        template<class Point, class A>
        void join_path(path<Point, A> const& p)
        {
            cut();
            splice(p);
        }

        template<class Path>
        auto add(Path const& p)
        {
            niji::render(p, sink(*this, true));
        }

        template<class Point, class A>
        void add(path<Point, A> const& p)
        {
            join_path(p);
        }

        template<class Point, class A>
        void splice(path<Point, A> const& p)
        {
            auto offset = nodes_base::size();
            join(p.begin(), p.end());
            _index_tags.reserve(_index_tags.size() + p._index_tags.size());
            for (auto const& i : p._index_tags)
                _index_tags.emplace_back(i.index + offset, i.tag);
        }

        template<class Point, class A>
        void reverse_splice(path<Point, A> const& p)
        {
            auto offset = nodes_base::size();
            join(p.rbegin(), p.rend());
            
            auto rit = p._index_tags.end(), rend = p._index_tags.begin();
            if (rit != rend)
            {
                _index_tags.reserve(_index_tags.size() + (rit - rend));
                char tag = end_tag::open | 4;
                auto remap = index_tag_t::remap(p.size(), tag);
                auto i = remap(*--rit);
                if (i.index)
                    _index_tags.emplace_back(i.index + offset, i.tag & 3);
                while (rit != rend)
                {
                    i = remap(*--rit);
                    _index_tags.emplace_back(i.index + offset, i.tag & 3);
                }
                if (!(tag & 4))
                    delimit(static_cast<end_tag>(tag));
            }
        }
        
        void unsafe_quad_to(Node const& pt1, Node const& pt2)
        {
            BOOST_ASSERT(!is_ended());
            _index_tags.emplace_back(nodes_base::size(), 2);
            nodes_base::push_back(pt1);
            nodes_base::push_back(pt2);
        }

        void unsafe_cubic_to(Node const& pt1, Node const& pt2, Node const& pt3)
        {
            BOOST_ASSERT(!is_ended());
            _index_tags.emplace_back(nodes_base::size(), 3);
            nodes_base::push_back(pt1);
            nodes_base::push_back(pt2);
            nodes_base::push_back(pt3);
        }

        void close()
        {
            delimit(end_tag::closed);
        }

        void cut()
        {
            delimit(end_tag::open);
        }

        void delimit(end_tag tag)
        {
            if (auto index = nodes_base::size())
            {
                if (_index_tags.empty() || _index_tags.back().index != index)
                    _index_tags.emplace_back(index, tag);
            }
        }
        
        void clear() noexcept
        {
            nodes_base::clear();
            _index_tags.clear();
        }

        void swap(path& other) noexcept
        {
            nodes_base::swap(other);
            _index_tags.swap(other._index_tags);
        }
        
        template<class Archive>
        void serialize(Archive& ar, unsigned version)
        {
            ar & _index_tags & *static_cast<nodes_base*>(this);
        }

    private:

        index_tag_container _index_tags;
    };
}

#endif