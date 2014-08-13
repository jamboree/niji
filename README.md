niji
====

2D graphics library for C++1y, path only, no color, no rendering.

## Introduction

Niji is a generic library for 2D path manipulation, in a sense like STL/Boost.Range, which provides containers, range generators, range adaptors and algorithms. Niji provides path containers, path generators (graphics), path adaptors (views) and path algorithms.

On the other hand, niji itself doesn't have the ability to render these paths, it's expected to work with some *real* graphic engine, which can do the rendering either on-screen or off-screen.

## Overview

Niji can be divided into these submodules:

- *core* - components to make up paths.
- *graphic* - common graphics, e.g. `ellipse`, `round_rect`, etc.
- *adapted* - adapted geometry path for Boost.Geometry.
- *view* - various path adaptors, e.g. `reverse`, `transform`, `stroke`, etc.
- *algorithm* - path algorithms, e.g. `bounds`, `length`, etc.
- *support* - traits, basic geometry, math, utility...
- *sink* - predefined backends, e.g. Skia, Direct2D, etc.

## Quick Example

```c++
using namespace boost::niji;                                                 
                                                                             
iterate(ellipse<double>({100, 100}, 50) | views::dash<double>({6, 2}), sink);
```
- all components reside in namespace `boost::niji`.
- `iterate(path, sink)` is a utility function that sends the path commands to sink.
- `ellipse` is a predefined path generator.
- `views::dash(pattern)` is a path adaptor that generates dash lines.
- `sink` is something that receives the path commands.

## Dependencies

- [Boost](http://www.boost.org/)

## License

    Copyright (c) 2014 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
    
