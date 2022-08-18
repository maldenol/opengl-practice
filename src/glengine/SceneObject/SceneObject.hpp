#ifndef GLENGINE_SCENEOBJECT_SCENEOBJECT_HPP
#define GLENGINE_SCENEOBJECT_SCENEOBJECT_HPP

// All the headers
#include "./Component/component.inc"

// STD
#include <memory>
#include <vector>

// GLM
#include <glm/glm.hpp>

namespace glengine {

// Scene object class
class SceneObject {
 private:
  // World orientation
  glm::vec3 _translate{};
  glm::vec3 _rotate{};
  glm::vec3 _scale{};

  glm::mat4 _modelMatrix{};

  // Components
  std::vector<std::shared_ptr<Component>> _componentPtrs{};

 public:
  // Constructors, assignment operators and destructor
  SceneObject() noexcept;
  SceneObject(const glm::vec3 &translate, const glm::vec3 &rotate, const glm::vec3 &scale,
              const std::vector<std::shared_ptr<Component>> &componentPtrs) noexcept;
  SceneObject(const glm::vec3 &translate, const glm::vec3 &rotate, const glm::vec3 &scale,
              int initComponentsCount, ...);
  SceneObject(const SceneObject &sceneObject) noexcept;
  SceneObject &operator=(const SceneObject &sceneObject) noexcept;
  SceneObject(SceneObject &&sceneObject) noexcept;
  SceneObject &operator=(SceneObject &&sceneObject) noexcept;
  ~SceneObject() noexcept;

  // Setters
  void setTranslate(const glm::vec3 &translate) noexcept;
  void setRotate(const glm::vec3 &rotate) noexcept;
  void setScale(const glm::vec3 &scale) noexcept;
  void setModelMatrix(const glm::mat4 &modelMatrix) noexcept;
  void setComponentPtrs(const std::vector<std::shared_ptr<Component>> &componentPtrs) noexcept;

  // Getters
  const glm::vec3                               &getTranslate() const noexcept;
  glm::vec3                                     &getTranslate() noexcept;
  const glm::vec3                               &getRotate() const noexcept;
  glm::vec3                                     &getRotate() noexcept;
  const glm::vec3                               &getScale() const noexcept;
  glm::vec3                                     &getScale() noexcept;
  const glm::mat4                               &getModelMatrix() const noexcept;
  glm::mat4                                     &getModelMatrix() noexcept;
  const std::vector<std::shared_ptr<Component>> &getComponentPtrs() const noexcept;
  std::vector<std::shared_ptr<Component>>       &getComponentPtrs() noexcept;

  // Other member functions
  void recalculateModelMatrix() noexcept;

  std::vector<std::shared_ptr<const Component>> getSpecificComponentPtrs(
      ComponentType type) const noexcept;
  std::vector<std::shared_ptr<Component>> getSpecificComponentPtrs(ComponentType type) noexcept;

  void updateShaderModelMatrix(const Mesh &mesh) const noexcept;
  void updateShaderLightColor(const Mesh &mesh, const BaseLight &light) const noexcept;
  void updateShaderExposure(const Mesh &mesh, float exposure) const noexcept;

  void render(float exposure = 1.0f) const noexcept;

  // Other static member functions
  static void updateShadersLights(const std::vector<SceneObject> &sceneObjects,
                                  const glm::vec3                &ambientColor = glm::vec3{},
                                  GLuint            directionalLightShadowMapShaderProgram = 0,
                                  GLuint            pointLightShadowMapShaderProgram       = 0,
                                  GLuint            spotLightShadowMapShaderProgram        = 0,
                                  const BaseCamera &camera = PerspectiveCamera{}) noexcept;
  static void updateShadersCamera(const std::vector<SceneObject> &sceneObjects,
                                  const BaseCamera               &camera) noexcept;
};

}  // namespace glengine

#endif
