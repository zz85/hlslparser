# Warning

Here lies a [emscripten](http://kripken.github.io/emscripten-site/index.html#) port of HLSLPaser. This port is basically modifications on Ignacio Castaño's (used for [The Witness](http://the-witness.net/news/2014/08/hlslparser/)) [improvements](https://github.com/Thekla/hlslparser) for Max McGuire's version of [HLSLParser](https://github.com/unknownworlds/hlslparser) (used for [Natural Selection](https://www.gamingonlinux.com/articles/natural-selection-2s-direct3d-hlsl-to-opengl-glsl-parser-open-sourced.3358)).

If you like this, you may also find my port for [GLSL Optimizer](https://github.com/zz85/hlslparser) interesting.

Usage

```sh
emcc src/*.cpp -o hlslparser.js -s EXPORTED_FUNCTIONS="['_parseHLSL']" --bind -O3
```

## [Demo](https://zz85.github.io/hlslparser) here.


hlslparser
==========

HLSL Parser and GLSL code generator

This is the code we used in Natural Selection 2 to convert HLSL shader code to
GLSL for use with OpenGL. The code is pulled from a larger codebase and has some
dependencies which have been replaced with stubs. These dependencies are all very
basic (array classes, memory allocators, etc.) so replacing them with our own
equivalent should be simple if you want to use this code.

The parser is designed to work with HLSL code written in the legacy Direct3D 9
style (e.g. D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY should be used with D3D11).
The parser works with cbuffers for uniforms, so in addition to generating GLSL,
there is a class provided for generating D3D9-compatible HLSL which doesn't
support cbuffers. The GLSL code requires version 3.1 for support of uniform blocks.
The parser is designed to catch all errors and generate "clean" GLSL which can
then be compiled without any errors.

The HLSL parsing is done though a basic recursive descent parser coded by hand
rather than using a parser generator. We believe makes the code easier to
understand and work with.

To get consistent results from Direct3D and OpenGL, our engine renders in OpenGL
"upside down". This is automatically added into the generated GLSL vertex shaders.

Although this code was written specifically for our use, we hope that it may be
useful as an educational tool or a base for someone who wants to do something
similar.