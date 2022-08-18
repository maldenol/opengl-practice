#ifndef GLENGINE_FILESYSTEM_LOADMAP_HPP
#define GLENGINE_FILESYSTEM_LOADMAP_HPP

// STD
#include <string>
#include <vector>

// OpenGL
#include <glad/glad.h>

namespace glengine {

// Loads 2D texture
GLuint loadMap2D(const std::string &filename, bool sRGB);

// Loads HDR 2D texture
GLuint loadMap2DHDR(const std::string &filename);

// Loads cubemap
GLuint loadMapCube(const std::vector<std::string> &filenames, bool sRGB);

// Loads HDR cubemap
GLuint loadMapCubeHDR(const std::vector<std::string> &filenames);

}  // namespace glengine

#endif
