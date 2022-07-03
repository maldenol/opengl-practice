#ifndef SCENEOBJECT_SCENEOBJECT_HPP
#define SCENEOBJECT_SCENEOBJECT_HPP

// STD
#include <memory>
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

// Base light struct
struct BaseLight {
  glm::vec3 color{};
  float     intensity{};

 protected:
  BaseLight(const glm::vec3 &color, float intensity) : color{color}, intensity{intensity} {}

 public:
  virtual ~BaseLight() noexcept {};
};

// Directional light struct
struct DirectionalLight : public BaseLight {
  glm::vec3 direction{};

  DirectionalLight(const glm::vec3 &color, float intensity, const glm::vec3 &direction)
      : BaseLight{color, intensity}, direction{direction} {}
};

// Point light struct
struct PointLight : public BaseLight {
  float linAttCoef{};
  float quadAttCoef{};

  PointLight(const glm::vec3 &color, float intensity, float linAttCoef, float quadAttCoef)
      : BaseLight{color, intensity}, linAttCoef{linAttCoef}, quadAttCoef{quadAttCoef} {}
};

// Spot light struct
struct SpotLight : public BaseLight {
  glm::vec3 direction{};

  float linAttCoef{};
  float quadAttCoef{};

  float angle{};
  float smoothAngle{};

  SpotLight(const glm::vec3 &color, float intensity, const glm::vec3 &direction, float linAttCoef,
            float quadAttCoef, float angle, float smoothAngle)
      : BaseLight{color, intensity},
        direction{direction},
        linAttCoef{linAttCoef},
        quadAttCoef{quadAttCoef},
        angle{angle},
        smoothAngle{smoothAngle} {}
};

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
};

// Scene object struct
struct SceneObject {
  // World orientation
  glm::vec3 translate{};
  glm::vec3 rotate{};
  glm::vec3 scale{1.0f};

  // Unity-like components
  std::shared_ptr<BaseLight> lightPtr{};
  std::shared_ptr<Mesh>      meshPtr{};
};

// VBO attribute struct
struct VBOAttribute {
  GLint       size{};
  GLenum      type{};
  GLboolean   normalized{};
  GLsizei     stride{};
  const void *pointer{};
};

// Loads 2D texture
GLuint loadTexture(const QString &filename);

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

// Prepares lights of given scene objects for given shader program
void prepareLights(GLuint shaderProgram, const std::vector<SceneObject> &sceneObjects);
// Renders scene object
void renderSceneObject(const SceneObject &sceneObject, const BaseCamera &camera);

}  // namespace glservice

#endif
