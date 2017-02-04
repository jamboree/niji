niji
====

Generic 2D graphics library for C++14.

## Introduction

Niji is a generic library for 2D graphical path manipulation, it provides containers, generators (graphics), adaptors (views) and algorithms for path, in a sense like STL/Boost.Range.

On the other hand, niji doesn't have the ability to render these paths on its own, you can think it as a generic front-end for arbitrary graphic engines which can do the real rendering.

## Overview

Niji can be divided into these submodules:

- *core* - path fundamentals.
- *container* - various path containers, e.g. `path`, `group`, etc.
- *graphic* - common graphics, e.g. `ellipse`, `round_rect`, etc.
- *geometry* - adapted geometry path for Boost.Geometry.
- *view* - various path adaptors, e.g. `inverse`, `transform`, `stroke`, etc.
- *make* - convenient path makers.
- *algorithm* - path algorithms, e.g. `bounds`, `length`, etc.
- *support* - traits, basic geometries, math...
- *sink* - predefined backends, e.g. Skia, Direct2D, etc.

## Quick Example

The following example shows how to output the path of a dashed ellipse:

```c++
using namespace niji;                                                 
                                                                             
render(ellipse<double>({100, 100}, 50) | views::dash<double>({6, 2}), sink);
```
- all components reside in namespace `niji`.
- `render(path, sink)` sends the path commands to sink.
- `ellipse<T>` is a predefined path generator.
- `views::dash<T>(pattern)` returns a path adaptor that generates dash lines.
- `sink` is something that handles the path commands.

## Documentation
- [Tutorial](https://github.com/jamboree/niji/wiki/Tutorial)
- Reference

## Dependencies

- [Boost](http://www.boost.org/)

## License

    Copyright (c) 2014-2017 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
    
