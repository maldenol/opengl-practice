#ifndef GLENGINE_SCENEOBJECT_MESH_LOADTEXTURE_HPP
#define GLENGINE_SCENEOBJECT_MESH_LOADTEXTURE_HPP

// STD
#include <string>
#include <vector>

// OpenGL
#include <glad/glad.h>

namespace glengine {

// Loads 2D texture
GLuint loadTexture(const std::string &filename, bool sRGB);

// Loads cubemap
GLuint loadCubemap(const std::vector<std::string> &filenames, bool sRGB);

}  // namespace glengine

#endif
