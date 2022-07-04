#ifndef GLENGINE_SCENE_MESH_MESH_HPP
#define GLENGINE_SCENE_MESH_MESH_HPP

// All the headers
#include "./loadTexture.hpp"

// STD
#include <vector>

// OpenGL
#include <glad/glad.h>

// GLM
#include <glm/glm.hpp>

namespace glengine {

// Mesh struct
class Mesh {
 public:
  // VBO attribute struct
  struct VBOAttribute {
    GLint       size{};
    GLenum      type{};
    GLboolean   normalized{};
    GLsizei     stride{};
    const void *pointer{};
  };

  // Material struct
  struct Material {
    // Texture struct
    struct Texture {
      int    index{};
      GLuint texture{};
    };

    float ambCoef{};
    float diffCoef{};
    float specCoef{};

    float glossiness{};

    float maxHeight{};

    std::vector<Texture> textures{};
  };

 private:
  GLuint  _vao{};
  GLuint  _vbo{};
  GLuint  _ebo{};
  GLsizei _indexCount{};

  GLuint   _shaderProgram{};
  Material _material{};

 public:
  Mesh() noexcept;
  Mesh(GLuint vao, GLuint vbo, GLuint ebo, GLsizei indexCount, GLuint shaderProgram,
       Material material) noexcept;
  Mesh(const std::vector<VBOAttribute> &vboAttributes, const std::vector<float> &vertexBuffer,
       const std::vector<GLuint> &indices, GLuint shaderProgram, const Material &material);
  Mesh(const Mesh &mesh) noexcept;
  Mesh &operator=(const Mesh &mesh) noexcept;
  Mesh(Mesh &&mesh) noexcept;
  Mesh &operator=(Mesh &&mesh) noexcept;
  ~Mesh() noexcept;

  void setVAO(GLuint vao) noexcept;
  void setVBO(GLuint vbo) noexcept;
  void setEBO(GLuint ebo) noexcept;
  void setIndexCount(GLsizei indexCount) noexcept;
  void setShaderProgram(GLuint shaderProgram) noexcept;
  void setMaterial(const Material &material) noexcept;

  GLuint          getVAO() const noexcept;
  GLuint         &getVAO() noexcept;
  GLuint          getVBO() const noexcept;
  GLuint         &getVBO() noexcept;
  GLuint          getEBO() const noexcept;
  GLuint         &getEBO() noexcept;
  GLsizei         getIndexCount() const noexcept;
  GLsizei        &getIndexCount() noexcept;
  GLuint          getShaderProgram() const noexcept;
  GLuint         &getShaderProgram() noexcept;
  const Material &getMaterial() const noexcept;
  Material       &getMaterial() noexcept;
};

// Generates vertex buffer based on vertices, normals, tangents and uvs
std::vector<float> generateVertexBuffer(std::vector<glm::vec3> vertices,
                                        std::vector<glm::vec3> normals,
                                        std::vector<glm::vec3> tangents,
                                        std::vector<glm::vec2> uvs);

// Calculates tangent by given positions and UVs of 3 points
glm::vec3 calculateTangent(const std::vector<glm::vec3> &pointPositions,
                           const std::vector<glm::vec2> &pointUVs);

// Generates plane mesh based on size, level-of-detail, shader program and textures
Mesh generatePlane(float size, int lod, GLuint shaderProgram,
                   const std::vector<Mesh::Material::Texture> &textures);
// Generates cube mesh based on size, level-of-detail, enableCubemap, shader program and textures
Mesh generateCube(float size, int lod, bool enableCubemap, GLuint shaderProgram,
                  const std::vector<Mesh::Material::Texture> &textures);
// Generates quad sphere mesh based on radius, level-of-detail, enableCubemap, shader program and textures
Mesh generateQuadSphere(float radius, int lod, bool enableCubemap, GLuint shaderProgram,
                        const std::vector<Mesh::Material::Texture> &textures);
// Generates UV sphere mesh based on radius, level-of-detail, shader program and textures
Mesh generateUVSphere(float radius, int lod, GLuint shaderProgram,
                      const std::vector<Mesh::Material::Texture> &textures);
// Generates icosphere mesh based on radius, shader program and textures
Mesh generateIcoSphere(float radius, GLuint shaderProgram,
                       const std::vector<Mesh::Material::Texture> &textures);

}  // namespace glengine

#endif
