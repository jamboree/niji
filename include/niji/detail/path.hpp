/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_DETAIL_PATH_HPP_INCLUDED
#define NIJI_DETAIL_PATH_HPP_INCLUDED

#include <boost/iterator/iterator_facade.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <niji/render.hpp>
#include <niji/support/command.hpp>
#include <niji/support/vector.hpp>
#include <niji/support/point.hpp>

namespace niji { namespace detail
{
    struct index_tag_t
    {
        std::size_t index;
        char tag;
        
        index_tag_t() = default; // for serialization

        index_tag_t(std::size_t index, char tag)
          : index(index), tag(tag)
        {}
        
        bool is_end_tag() const
        {
            return !(tag >> 1);
        }

        template<class Archive>
        void serialize(Archive & ar, unsigned version)
        {
            ar & index & tag;
        }

        static auto remap(std::size_t max, char& end)
        {
            return [max, &end](index_tag_t ret)
            {
                ret.index = max - ret.index;
                if (ret.is_end_tag())
                    std::swap(ret.tag, end);
                else // curve-tag
                    ret.index -= 1 + (ret.tag == 3);
                return ret;
            };
        }

        static auto remap_no_end(std::size_t max)
        {
            return [max](index_tag_t ret)
            {
                ret.index = max - (ret.index + 1 + (ret.tag == 3));
                return ret;
            };
        }
    };
    
    inline auto index_tag_is_end()
    {
        return [](index_tag_t const& a)
        {
            return a.is_end_tag();
        };
    }

    template<class Sink, class Nodes, class IndexTags>
    bool path_render_impl
    (
        Sink& sink
      , Nodes const& nodes
      , IndexTags const& index_tags
      , bool heading = true
    )
    {
        using namespace command;

        auto it(nodes.begin()), end(nodes.end());
        for (auto const& i : index_tags)
        {
            auto tagged(nodes.begin() + i.index);
            if (it != tagged)
            {
                if (heading)
                    sink(move_to, *it);
                else
                    sink(line_to, *it);
                while (++it != tagged)
                    sink(line_to, *it);
            }
            else if (heading)
                continue;
            switch (i.tag)
            {
            case end_tag::closed:
                sink(end_closed);
                heading = true;
                break;
            case end_tag::open:
                sink(end_open);
                heading = true;
                break;
            case 2:
                sink(quad_to, *it, *(it + 1));
                it += 2;
                heading = false;
                break;
            case 3:
                sink(cubic_to, *it, *(it + 1), *(it + 2));
                it += 3;
                heading = false;
                break;
            }
        }
        if (it != end)
        {
            if (heading)
                sink(move_to, *it);
            else
                sink(line_to, *it);
            while (++it != end)
                sink(line_to, *it);
            return true; // needs ending
        }
        return !heading; // ended
    }

    template<class NodeIt>
    bool path_is_box(NodeIt it, NodeIt const& end)
    {
        using node_t = typename boost::iterator_value<NodeIt>::type;
        using coord_t = typename boost::geometry::coordinate_type<node_t>::type;

        if (end - it != 4)
            return false;

        int i;
        point<coord_t> o(*it), prev(*++it);
        if (prev.x == o.x)
            i = 1;
        else if (prev.y == o.y)
            i = 0;
        else
            return false;
            
        point<coord_t> curr(*++it);
        coord_t *p = &prev.x, *c = &curr.x;
        if (p[i] != c[i])
            return false;
        prev = curr, curr = *++it, i = !i;
        if (p[i] != c[i])
            return false;
        prev = curr, curr = o, i = !i;

        return p[i] == c[i];
    }
    
    template<class NodeIt, class IndexIt>
    struct pathlet
    {
        using point_type = typename NodeIt::value_type;
        using iterator = NodeIt;
        using const_iterator = NodeIt;

        pathlet() = default;

        pathlet(NodeIt const& begin, NodeIt const& end, IndexIt const& tit, IndexIt const& tend, std::size_t offset)
          : _begin(begin), _end(end), _index_tags(tit, tend)
          , _offset(offset)
        {}

        iterator begin() const
        {
            return _begin + (_offset >> 1);
        }

        iterator end() const
        {
            return _end;
        }

        bool empty() const
        {
            return begin() == _end;
        }

        bool is_closed() const
        {
            return _offset & 1;
        }

        bool is_box() const
        {
            return path_is_box(begin(), end());
        }

        template<class Sink>
        void render(Sink& sink) const
        {
            render_impl(sink, _begin, begin(), _end, _index_tags, is_closed());
        }
        
        template<class Sink>
        void inverse_render(Sink& sink) const
        {
            namespace rng = ::boost::adaptors;
            
            boost::reverse_iterator<NodeIt> it(_end), end(begin());
            render_impl(sink, it, it, end, rng::transform(rng::reverse(_index_tags),
                index_tag_t::remap_no_end(_end - _begin)), is_closed());
        }

    private:
        
        template<class Sink, class It, class IndexTags>
        static void render_impl(Sink& sink, It origin, It it, It end, IndexTags const& index_tags, bool is_closed)
        {
            using namespace command;

            if (it == end)
                return;
            sink(move_to, *it++);
            for (auto const& i : index_tags)
            {
                auto tagged(origin + i.index);
                for ( ; it != tagged; ++it)
                    sink(line_to, *it);
                switch (i.tag)
                {
                case 2:
                    sink(quad_to, *it, *(it + 1));
                    it += 2;
                    break;
                case 3:
                    sink(cubic_to, *it, *(it + 1), *(it + 2));
                    it += 3;
                    break;
                }
            }
            for ( ; it != end; ++it)
                sink(line_to, *it);
            if (is_closed)
                sink(end_closed);
            else
                sink(end_open);
        }

        NodeIt _begin, _end;
        boost::iterator_range<IndexIt> _index_tags;
        std::size_t _offset;
    };

    template<class NodeIt, class IndexIt>
    struct path_partition
    {
        using value_type = pathlet<NodeIt, IndexIt>;
        using reference = value_type;
        
        path_partition(NodeIt const& begin, NodeIt const& end, IndexIt const& tbegin, IndexIt const& tend)
          : _begin(begin), _end(end), _tbegin(tbegin), _tend(tend)
        {}

        struct iterator
          : boost::iterator_facade
            <
                iterator
              , value_type
              , boost::forward_traversal_tag
              , reference
            >
        {
            iterator() {}

            iterator(path_partition const* rng, IndexIt const& tit)
              : _rng(rng)
              , _tit(std::find_if(tit, rng->_tend, index_tag_is_end()))
              , _tlast(tit), _it(_tit == rng->_tend? _rng->_end : _rng->_begin + _tit->index)
            {}

            void increment()
            {
                _tlast = _tit;
                IndexIt tend(_rng->_tend);
                if (_tit == tend)
                {
                    _it = {};
                    return;
                }
                _tit = std::find_if(++_tit, tend, index_tag_is_end());
                _it = _tit == tend? _rng->_end : _rng->_begin + _tit->index;
            }

            bool equal(iterator const& other) const
            {
                return _it == other._it;
            }
            
            reference dereference() const
            {
                std::size_t offset = 0;
                auto tlast = _tlast;
                if (tlast != _tit && tlast->is_end_tag())
                {
                    offset = tlast->index << 1;
                    ++tlast;
                }
                offset |= _tit != _rng->_tend && !_tit->tag;
                return {_rng->_begin, _it, tlast, _tit, offset};
            }

        private:

            path_partition const* _rng;
            IndexIt _tit, _tlast;
            NodeIt _it;
        };

        using const_iterator = iterator;

        iterator begin() const
        {
            return {this, _tbegin};
        }

        iterator end() const
        {
            return {};
        }

    private:

        NodeIt const _begin, _end;
        IndexIt const _tbegin, _tend;
    };

    template<class NodeIt, class IndexIt>
    struct incomplete_path
    {
        using point_type = typename NodeIt::value_type;

        incomplete_path(NodeIt const& begin, NodeIt const& end, IndexIt const& tbegin, IndexIt const& tend)
          : _nodes(begin, end), _index_tags(tbegin, tend)
        {}

        template<class Sink>
        void render(Sink& sink) const
        {
            path_render_impl(sink, _nodes, _index_tags, false);
        }

    private:

        boost::iterator_range<NodeIt> _nodes;
        boost::iterator_range<IndexIt> _index_tags;
    };
}}

#endif