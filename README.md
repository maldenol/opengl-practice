# opengl-practice
opengl-practice is a collection of applications and also a graphics library
which I made to practice at OpenGL (4.6 core) and rendering techniques.
<br/>
<br/>

## Description
<details>
<summary>Click to see the description</summary>
<br/>

In src/ directory there are 6 subprojects: 5 programs and a library.
Each program inherits all the content from previous one.
They are made as a demonstation of techniques or the library abilities.
The library itself can handle next targets:
1. Window creation and initialization of OpenGL context.
2. Shader utilities which includes compile functions and also shader watcher -
a thread-based function that looks after shader files and recompiles them after
changes were made.
3. SceneObject class with component-based structure. It also includes Component class,
which is inherited by BaseCamera (and then OrthographicCamera and PerspectiveCamera),
BaseLight (and then DirectionalLight, PointLight and SpotLight)
and Mesh (and then Material (and then Texture)).
There is also 3 camera controller classes for 3, 4 and 6 degrees of freedom;
and also procedural Mesh generation algorithms with specific level of detail.
4. Filesystem utilities and different texture loading functions.
5. Function for debugging OpenGL.

</details>
</br>

### Demo
[![Here must be demo](<image url>)](<video url> "opengl-practice demo")
</br>
</br>

## Used techniques
<details>
<summary>Click to see used techniques</summary>
<br/>

1. 1-triangle:
  - Vertex and fragment shaders
  - Texture mapping: UV and triplanar
  - Texture filtering: bilinear and trilinear
2. 2-camera
  - Camera and controls
3. 3-basic_lighting
  - Blinn-Phong lighting model
  - Materials
  - Multiple direct light sources: directional, point and spot
4. 4-advanced_opengl
  - Depth testing
  - Stencil testing (made objects outline)
  - Blending
  - Face culling
  - Bump mapping: normal and parallax
  - Environment mapping
  - Framebuffers and postprocessing
  - Cubemaps
  - Geometry shaders
  - Instancing
  - Anti-aliasing (MSAA)
  - Tessellation and compute shaders
5. 5-advanced_lighting
  - Gamma correction
  - Shadow mapping
  - HDR and tone mapping: Reinhard and exposure

TODO:
  - Physically Based Rendering
    - Cook-Torrance Reflectance Model
    - Image Based Lighting
    - Subsurface scattering
  - Text and fonts
  - Model loading
    - Make Mesh have children
    - Make SceneObject static functions also affect childen of Mesh
    - Load models using Assimp library
  - Deffered rendering
  - Effects
    - Fog
    - Halo, shaft, backscattering
    - Optical distortions
    - Bloom
    - SSAO
    - Motion blur
    - Depth of field

</details>
</br>

## Building
<details>
<summary>Click to see how to build it</summary>
<br/>

The whole project is written in C++ using:
1. OpenGL 4.6 (GLFW and GLAD)
2. Qt 5
3. GLM
4. stb_image

Therefore before building you need to satisfy all the dependencies
by taking these steps:
1. Install Qt using your system package manager or [official installer](https://www.qt.io/download).
2. Download [GLFW](https://www.glfw.org/download), build it,
then place the static library file in ```lib/```
and header directories in ```include/```.
3. Download [GLAD](https://glad.dav1d.de/),
build it as static or shared library and place it in ```lib/```
then place the header directory in ```include/```.
4. Download [GLM](https://glm.g-truc.net/0.9.8/index.html)
and place it in ```include/```.
5. Download [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
and place it in ```include/stb/```.

To build the application run these commands from the project root:
```bash
cmake .
cmake --build . --target <project>
```
, where <project> is name of any directory in ```src/```.  
You will have your projects builded in ```bin/``` directory.

</details>
</br>

## License
opengl-practice is released into public domain.
</br>
