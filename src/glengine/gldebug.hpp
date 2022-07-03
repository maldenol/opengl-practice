#ifndef GLENGINE_GLDEBUG_HPP
#define GLENGINE_GLDEBUG_HPP

// OpenGL
#include <glad/glad.h>

namespace glengine {

// Outputs OpenGL error message and line
GLenum glCheckError(const char *file, int line);

}  // namespace glengine

#define GL_CHECK_ERROR() glCheckError(__FILE__, __LINE__)

#endif
