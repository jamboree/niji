/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BOOST_NIJI_EXAMPLE_SVG_HPP_INCLUDED
#define BOOST_NIJI_EXAMPLE_SVG_HPP_INCLUDED

#include <iostream>
#include <sstream>
#include <cstdint>
#include <boost/optional/optional.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/niji/iterate.hpp>
#include <boost/niji/support/command.hpp>
#include <boost/niji/support/point.hpp>
#include <boost/niji/support/transform/affine.hpp>
#include <boost/niji/sink/svg.hpp>

namespace boost { namespace niji { namespace svg
{
    struct rgb
    {
        std::uint8_t r, g, b;
        
        rgb() : r(), g(), b() {}
        
        rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b)
          : r(r), g(g), b(b)
        {}
    };

    struct paint
    {
        rgb color;
        double opacity;
        
        paint(rgb color, double opacity = 1.0)
          : color(color), opacity(opacity)
        {}
    };
    
    enum class cap_style
    {
        butt, round, square
    };
    
    enum class join_style
    {
        bevel, round, miter
    };
    
    struct pen
    {
        double width;
        svg::paint paint;
        std::vector<double> dash;
        double offset = 0;
        cap_style cap = cap_style::butt;
        join_style join = join_style::bevel;
        double miter_limit = 4.0;

        pen(double width, svg::paint const& paint)
          : width(width), paint(paint)
        {}
    };
    
    enum class font_style
    {
        normal, italic, oblique
    };
    
    enum font_weight
    {
        normal = 400, bold = 700
    };
    
    struct font
    {
        double size;
        std::string family;
        font_style style;
        int weight;
        
        font(double size, std::string family = {}, font_style style = font_style::normal, int weight = font_weight::normal)
          : size(size), family(std::move(family)), style(style), weight(weight)
        {}
    };

    void save_paint(std::ostream& out, paint const& p, char const* type)
    {
        auto c = p.color;
        out << type << "=\"rgb(" << unsigned(c.r) << ',' << unsigned(c.g) << ',' << unsigned(c.b) << ")\"";
        if (p.opacity < 1)
            out << ' ' << type << "-opacity=\"" << p.opacity << '\"';
    }
    
    void save_pen(std::ostream& out, pen const& p)
    {
        char const* cap[] = {"butt", "round", "square"};
        char const* join[] = {"bevel", "round", "miter"};
        out << "stroke-width=\"" << p.width << "\" ";
        save_paint(out, p.paint, "stroke");
        out << " stroke-linecap=\"" << cap[int(p.cap)] << '\"'
            << " stroke-linejoin=\"" << join[int(p.join)] << '\"';
        if (p.join == join_style::miter)
            out << " stroke-miterlimit=\"" << p.miter_limit << '\"';
        auto it = p.dash.begin(), end = p.dash.end();
        if (it != end)
        {
            out << " stroke-dasharray=\"" << *it;
            while (++it != end)
                out << ',' << *it;
            out << '\"';
            if (p.offset)
                out << " stroke-dashoffset=\"" << p.offset << '\"';
        }
    }

    template<class Ostream>
    struct basic_canvas
    {
        Ostream& out;
        
        basic_canvas(Ostream& out, unsigned x, unsigned y)
          : out(out)
        {
            fill(nullptr);
            stroke(nullptr);
            
            out << "<?xml version=\"1.0\" standalone=\"no\"?>"
               "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">"
               "<svg width=\"" << x << "px\" height=\"" << y << "px\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">";
        }
        
        void fill(std::nullptr_t)
        {
            _brush = "fill=\"none\"";
        }
        
        void fill(paint const& b)
        {
            std::stringstream s;
            save_paint(s, b, "fill");
            _brush = s.str();
        }
        
        void stroke(std::nullptr_t)
        {
            _pen = "stroke=\"none\"";
        }

        void stroke(pen const& p)
        {
            std::stringstream s;
            save_pen(s, p);
            _pen = s.str();
        }
        
        template<class Path>
        void draw(Path const& path)
        {
            out << "<path d=\"";
            iterate(path, basic_svg_sink<Ostream>(out));
            out << "\" " << _brush << ' ' << _pen << " />";
        }

        void text(dpoint const& pt, std::string const& str, font const& f)
        {
            out << "<text x=\"" << pt.x << "\" y=\"" << pt.y << "\" ";
            save_font(out, f);
            out << " >" << str << "</text>";
        }
        
        template<class T, class F>
        void transform(transforms::affine<T> const& trans, F&& f)
        {
            out << "<g transform=\"matrix(" << trans.sx << ',' << trans.shy << ',' << trans.shx << ',' << trans.sy << ',' << trans.tx << ',' << trans.ty << ")\" >";
            f(*this);
            out << "</g>";
        }
    
        static void save_font(Ostream& out, font const& f)
        {
            char const* style[] = {"normal", "italic", "oblique"};
            out << "font-size=\"" << f.size << '\"';
            if (!f.family.empty())
                out << " font-family=\"" << f.family << '\"';
            out << " font-style=\"" << style[int(f.style)] << '\"';
            out << " font-weight=\"" << f.weight << '\"';
        }

        ~basic_canvas()
        {
            out << "</svg>";
        }
        
    private:
        
        std::string _brush;
        std::string _pen;
    };
    
    using canvas = basic_canvas<std::ostream>;
}}}

#endif
