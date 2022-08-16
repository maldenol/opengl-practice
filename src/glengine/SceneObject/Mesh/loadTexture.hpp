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

// Loads HDR 2D texture
GLuint loadTextureHDR(const std::string &filename);

// Loads cubemap
GLuint loadCubemap(const std::vector<std::string> &filenames, bool sRGB);

// Loads HDR cubemap
GLuint loadCubemapHDR(const std::vector<std::string> &filenames);

}  // namespace glengine

#endif
