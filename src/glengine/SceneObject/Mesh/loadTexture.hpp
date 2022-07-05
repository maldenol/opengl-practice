#ifndef GLENGINE_SCENEOBJECT_MESH_LOADTEXTURE_HPP
#define GLENGINE_SCENEOBJECT_MESH_LOADTEXTURE_HPP

// OpenGL
#include <glad/glad.h>

// Qt5
#include <QString>

namespace glengine {

// Loads 2D texture
GLuint loadTexture(const QString &filename);

}  // namespace glengine

#endif
