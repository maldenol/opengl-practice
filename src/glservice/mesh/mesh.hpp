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

// Texture struct
struct Texture {
  int    index{};
  GLuint texture{};
};

// Material struct
struct Material {
  float ambCoef{0.15f};
  float diffCoef{0.6f};
  float specCoef{0.3f};
  float glossiness{1.0f};
  float maxHeight{};

  std::vector<Texture> textures{};
};

// Mesh struct
struct Mesh {
  GLuint  vao{};
  GLuint  vbo{};
  GLsizei indexCount{};

  GLuint   shaderProgram{};
  Material material{};

  glm::vec3 translate{};
  glm::vec3 rotate{};
  glm::vec3 scale{1.0f};
};

// VBO attribute struct
struct VBOAttribute {
  GLint       size{};
  GLenum      type{};
  GLboolean   normalized{};
  GLsizei     stride{};
  const void *pointer{};
};

// Generates mesh based on VBO attributes, vertex buffer, indices, shader program and textures
Mesh generateMesh(const std::vector<VBOAttribute> &vboAttributes,
                  const std::vector<float> &vertexBuffer, const std::vector<GLuint> &indices,
                  GLuint shaderProgram, const std::vector<Texture> &textures);

// Generates vertex buffer based on vertices, normals, tangents and uvs
std::vector<float> generateVertexBuffer(std::vector<glm::vec3> vertices,
                                        std::vector<glm::vec3> normals,
                                        std::vector<glm::vec3> tangents,
                                        std::vector<glm::vec2> uvs);

// Calculates tangent by given positions and UVs of 3 points
glm::vec3 calculateTangent(const std::vector<glm::vec3> &pointPositions,
                           const std::vector<glm::vec2> &pointUVs);

// Generates plane mesh based on size, level-of-detail, shader program and textures
Mesh generatePlane(float size, int lod, GLuint shaderProgram, const std::vector<Texture> &textures);
// Generates cube mesh based on size, level-of-detail, enableCubemap, shader program and textures
Mesh generateCube(float size, int lod, bool enableCubemap, GLuint shaderProgram,
                  const std::vector<Texture> &textures);
// Generates quad sphere mesh based on radius, level-of-detail, enableCubemap, shader program and textures
Mesh generateQuadSphere(float radius, int lod, bool enableCubemap, GLuint shaderProgram,
                        const std::vector<Texture> &textures);
// Generates UV sphere mesh based on radius, level-of-detail, shader program and textures
Mesh generateUVSphere(float radius, int lod, GLuint shaderProgram,
                      const std::vector<Texture> &textures);
// Generates icosphere mesh based on radius, shader program and textures
Mesh generateIcoSphere(float radius, GLuint shaderProgram, const std::vector<Texture> &textures);

// Loads 2D texture
GLuint loadTexture(const QString &filename);

// Renders mesh
void renderMesh(const Mesh &mesh, const BaseCamera &camera);

}  // namespace glservice

#endif
