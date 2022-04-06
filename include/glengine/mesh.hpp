#ifndef GLENGINE_MESH_HPP
#define GLENGINE_MESH_HPP

// STD
#include <vector>

// OpenGL
#include <glad/glad.h>

namespace glengine {

// Initializes mesh based on vertices and indices
GLuint initMesh(const std::vector<float>  &vertices,
                const std::vector<GLuint> &indices);

// Draws mesh
void drawMesh(GLuint vao, GLsizei vertexCount, GLuint shaderProgram);

};  // namespace glengine

#endif