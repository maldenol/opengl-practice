#ifndef GLENGINE_SCENEOBJECT_MESH_MESH_HPP
#define GLENGINE_SCENEOBJECT_MESH_MESH_HPP

// All the headers
#include "./loadTexture.hpp"

// STD
#include <memory>
#include <vector>

// OpenGL
#include <glad/glad.h>

// GLM
#include <glm/glm.hpp>

namespace glengine {

// Global constants

constexpr float kInitAmbCoef    = 0.15f;
constexpr float kInitDiffCoef   = 0.6f;
constexpr float kInitSpecCoef   = 0.3f;
constexpr float kInitGlossiness = 1.0f;
constexpr float kInitMaxHeight  = 0.0f;

// Mesh class
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
      GLuint texture{};
      int    index{};
      bool   isCubemap{};
    };

    float ambCoef{};
    float diffCoef{};
    float specCoef{};

    float glossiness{};

    float maxHeight{};

    std::vector<std::shared_ptr<Texture>> texturePtrs{};
  };

 private:
  GLuint  _vao{};
  GLuint  _vbo{};
  GLuint  _ebo{};
  GLsizei _indexCount{};

  GLuint _shaderProgram{};

  std::shared_ptr<Material> _materialPtr{};

 public:
  // Constructors, assignment operators and destructor
  Mesh() noexcept;
  Mesh(GLuint vao, GLuint vbo, GLuint ebo, GLsizei indexCount, GLuint shaderProgram,
       const std::shared_ptr<Material> &materialPtr) noexcept;
  Mesh(const std::vector<VBOAttribute> &vboAttributes, const std::vector<float> &vertexBuffer,
       const std::vector<GLuint> &indices, GLuint shaderProgram,
       const std::shared_ptr<Material> &materialPtr);
  Mesh(const Mesh &mesh) noexcept;
  Mesh &operator=(const Mesh &mesh) noexcept;
  Mesh(Mesh &&mesh) noexcept;
  Mesh &operator=(Mesh &&mesh) noexcept;
  ~Mesh() noexcept;

  // Setters
  void setVAO(GLuint vao) noexcept;
  void setVBO(GLuint vbo) noexcept;
  void setEBO(GLuint ebo) noexcept;
  void setIndexCount(GLsizei indexCount) noexcept;
  void setShaderProgram(GLuint shaderProgram) noexcept;
  void setMaterialPtr(const std::shared_ptr<Material> &materialPtr) noexcept;

  // Getters
  GLuint                           getVAO() const noexcept;
  GLuint                          &getVAO() noexcept;
  GLuint                           getVBO() const noexcept;
  GLuint                          &getVBO() noexcept;
  GLuint                           getEBO() const noexcept;
  GLuint                          &getEBO() noexcept;
  GLsizei                          getIndexCount() const noexcept;
  GLsizei                         &getIndexCount() noexcept;
  GLuint                           getShaderProgram() const noexcept;
  GLuint                          &getShaderProgram() noexcept;
  const std::shared_ptr<Material> &getMaterialPtr() const noexcept;
  std::shared_ptr<Material>       &getMaterialPtr() noexcept;

  // Other member functions
  void render(unsigned int instanceCount) const noexcept;

  bool isComplete() const noexcept;
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
Mesh generatePlane(float size, unsigned int lod, GLuint shaderProgram,
                   const std::vector<std::shared_ptr<Mesh::Material::Texture>> &texturePtrs);
// Generates cube mesh based on size, level-of-detail, enableCubemap, shader program and textures
Mesh generateCube(float size, unsigned int lod, bool enableCubemap, GLuint shaderProgram,
                  const std::vector<std::shared_ptr<Mesh::Material::Texture>> &texturePtrs);
// Generates quad sphere mesh based on radius, level-of-detail, enableCubemap, shader program and textures
Mesh generateQuadSphere(float radius, unsigned int lod, bool enableCubemap, GLuint shaderProgram,
                        const std::vector<std::shared_ptr<Mesh::Material::Texture>> &texturePtrs);
// Generates UV sphere mesh based on radius, level-of-detail, shader program and textures
Mesh generateUVSphere(float radius, unsigned int lod, GLuint shaderProgram,
                      const std::vector<std::shared_ptr<Mesh::Material::Texture>> &texturePtrs);
// Generates icosphere mesh based on radius, shader program and textures
Mesh generateIcoSphere(float radius, GLuint shaderProgram,
                       const std::vector<std::shared_ptr<Mesh::Material::Texture>> &texturePtrs);

}  // namespace glengine

#endif
