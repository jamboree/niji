/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2015-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef NIJI_SINK_NVPR_HPP_INCLUDED
#define NIJI_SINK_NVPR_HPP_INCLUDED

// We don't include any GL binding here so that the user can choose
// which binding he want to use. e.g.
// #include <GL/glew.h>

#include <niji/support/command.hpp>
#include <niji/support/point.hpp>
#include <vector>

namespace niji
{
    struct nvpr_sink
    {
        using point_t = point<GLfloat>;

        std::vector<GLubyte> commands;
        std::vector<point_t> points;

        void operator()(move_to_t, point_t const& pt)
        {
            commands.push_back(GLubyte(GL_MOVE_TO_NV));
            points.push_back(pt);
        }

        void operator()(line_to_t, point_t const& pt)
        {
            commands.push_back(GLubyte(GL_LINE_TO_NV));
            points.push_back(pt);
        }

        void operator()(quad_to_t, point_t const& pt1, point_t const& pt2)
        {
            commands.push_back(GLubyte(GL_QUADRATIC_CURVE_TO_NV));
            points.push_back(pt1);
            points.push_back(pt2);
        }

        void operator()(cubic_to_t, point_t const& pt1, point_t const& pt2, point_t const& pt3)
        {
            commands.push_back(GLubyte(GL_CUBIC_CURVE_TO_NV));
            points.push_back(pt1);
            points.push_back(pt2);
            points.push_back(pt3);
        }
        
        void operator()(end_open_t) {}

        void operator()(end_closed_t)
        {
            commands.push_back(GLubyte(GL_CLOSE_PATH_NV));
        }

        void flush(GLuint path)
        {
            glPathCommandsNV
            (
                path,
                GLsizei(commands.size()),
                commands.data(),
                GLsizei(points.size() << 1),
                GL_FLOAT,
                points.data()
            );
            commands.clear();
            points.clear();
        }
    };
}

#endif