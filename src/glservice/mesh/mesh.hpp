#ifndef MESH_MESH_HPP
#define MESH_MESH_HPP

// STD
#include <vector>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

// Qt5
#include <QString>

// "glservice" internal library
#include "../camera/camera.hpp"

namespace glservice {

// Mesh struct
struct Mesh {
  GLuint              vao{};
  GLuint              vbo{};
  GLsizei             indexCount{};
  std::vector<GLuint> textures{};
  GLuint              shaderProgram{};
  glm::vec3           translate{};
  glm::vec3           rotate{};
  glm::vec3           scale{1.0f};
};

// VBO attribute struct
struct VBOAttribute {
  GLint       size{};
  GLenum      type{};
  GLboolean   normalized{};
  GLsizei     stride{};
  const void *pointer{};
};

// Generates mesh based on VBO attributes, vertex buffer, indices, textures and shader program
Mesh generateMesh(const std::vector<VBOAttribute> &vboAttributes,
                  const std::vector<float> &vertexBuffer, const std::vector<GLuint> &indices,
                  const std::vector<GLuint> &textures, GLuint shaderProgram);

// Generates vertex buffer based on vertices, normals and uvs
std::vector<float> generateVertexBuffer(std::vector<glm::vec3> vertices,
                                        std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs);

// Generates plane mesh based on size, level-of-detail, textures and shader program
Mesh generatePlane(float size, int lod, const std::vector<GLuint> &textures, GLuint shaderProgram);
// Generates cube mesh based on size, level-of-detail, textures and shader program
Mesh generateCube(float size, int lod, const std::vector<GLuint> &textures, GLuint shaderProgram);
// Generates quad sphere mesh based on radius, level-of-detail, textures and shader program
Mesh generateQuadSphere(float radius, int lod, const std::vector<GLuint> &textures,
                        GLuint shaderProgram);
// Generates UV sphere mesh based on radius, level-of-detail, textures and shader program
Mesh generateUVSphere(float radius, int lod, const std::vector<GLuint> &textures,
                      GLuint shaderProgram);
// Generates icosphere mesh based on radius, textures and shader program
Mesh generateIcoSphere(float radius, const std::vector<GLuint> &textures, GLuint shaderProgram);

// Loads 2D texture
GLuint loadTexture(const QString &filename);

// Renders mesh
void renderMesh(const Mesh &mesh, const BaseCamera &camera);

}  // namespace glservice

#endif
