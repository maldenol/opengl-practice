#ifndef GLENGINE_SCENEOBJECT_COMPONENT_MESH_MESH_HPP
#define GLENGINE_SCENEOBJECT_COMPONENT_MESH_MESH_HPP

// STD
#include <memory>
#include <vector>

// OpenGL
#include <glad/glad.h>

// GLM
#include <glm/glm.hpp>

// "glengine" internal library
#include "../Component.hpp"

namespace glengine {

// Global constants
static constexpr glm::vec3 kRight{1.0f, 0.0f, 0.0f};
static constexpr glm::vec3 kUp{0.0f, 1.0f, 0.0f};
static constexpr glm::vec3 kForward{0.0f, 0.0f, -1.0f};

// Mesh class
class Mesh : public Component {
 public:
  // VBO attribute struct
  struct VBOAttribute {
    GLint       size{};
    GLenum      type{};
    GLboolean   normalized{};
    GLsizei     stride{};
    const void *pointer{};
  };

  // Material class
  class Material {
   public:
    // Texture class
    class Texture {
     private:
      GLuint _name{};
      int    _unit{};
      bool   _isCubemap{};

     public:
      // Constructors, assignment operators and destructor
      Texture() noexcept;
      Texture(GLuint name, int unit, bool isCubemap) noexcept;
      Texture(const Texture &texture) noexcept;
      Texture &operator=(const Texture &texture) noexcept;
      Texture(Texture &&texture) noexcept;
      Texture &operator=(Texture &&texture) noexcept;
      ~Texture() noexcept;

      // Setters
      void setName(GLuint name) noexcept;
      void setUnit(int unit) noexcept;
      void setIsCubemap(bool isCubemap) noexcept;

      // Getters
      GLuint  getName() const noexcept;
      GLuint &getName() noexcept;
      int     getUnit() const noexcept;
      int    &getUnit() noexcept;
      bool    getIsCubemap() const noexcept;
      bool   &getIsCubemap() noexcept;
    };

   private:
    std::vector<std::shared_ptr<Texture>> _texturePtrs{};

    float _parallaxStrength{};

   public:
    // Constructors, assignment operators and destructor
    Material() noexcept;
    Material(const std::vector<std::shared_ptr<Texture>> &texturePtrs,
             float                                        parallaxStrength = 0.0f) noexcept;
    Material(const Material &material) noexcept;
    Material &operator=(const Material &material) noexcept;
    Material(Material &&material) noexcept;
    Material &operator=(Material &&material) noexcept;
    ~Material() noexcept;

    // Setters
    void setTexturePtrs(const std::vector<std::shared_ptr<Texture>> &texturePtrs) noexcept;
    void setParallaxStrength(float parallaxStrength) noexcept;

    // Getters
    const std::vector<std::shared_ptr<Texture>> &getTexturePtrs() const noexcept;
    std::vector<std::shared_ptr<Texture>>       &getTexturePtrs() noexcept;
    float                                        getParallaxStrength() const noexcept;
    float                                       &getParallaxStrength() noexcept;
  };

 private:
  GLuint  _vao{};
  GLuint  _vbo{};
  GLuint  _ebo{};
  GLsizei _indexCount{};

  GLsizei _instanceCount{};

  GLint _patchVertices{};

  GLuint _shaderProgram{};

  std::shared_ptr<Material> _materialPtr{};

 public:
  // Constructors, assignment operators and destructor
  Mesh() noexcept;
  Mesh(GLuint vao, GLuint vbo, GLuint ebo, GLsizei indexCount, GLsizei instanceCount,
       GLint _patchVertices, GLuint shaderProgram,
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
  void setInstanceCount(GLsizei instanceCount) noexcept;
  void setPatchVertices(GLint patchVertices) noexcept;
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
  GLsizei                          getInstanceCount() const noexcept;
  GLsizei                         &getInstanceCount() noexcept;
  GLint                            getPatchVertices() const noexcept;
  GLint                           &getPatchVertices() noexcept;
  GLuint                           getShaderProgram() const noexcept;
  GLuint                          &getShaderProgram() noexcept;
  const std::shared_ptr<Material> &getMaterialPtr() const noexcept;
  std::shared_ptr<Material>       &getMaterialPtr() noexcept;

  // Other member functions
  void render() const noexcept;

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
