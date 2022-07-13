#ifndef GLENGINE_SCENEOBJECT_SCENEOBJECT_HPP
#define GLENGINE_SCENEOBJECT_SCENEOBJECT_HPP

// All the headers
#include "./Camera/camera.hpp"
#include "./Light/light.hpp"
#include "./Mesh/Mesh.hpp"

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

  // Unity-like components
  std::shared_ptr<BaseLight> _lightPtr{};
  std::shared_ptr<Mesh>      _meshPtr{};

 public:
  // Constructors, assignment operators and destructor
  SceneObject() noexcept;
  SceneObject(const glm::vec3 &translate, const glm::vec3 &rotate, const glm::vec3 &scale,
              const std::shared_ptr<BaseLight> &lightPtr,
              const std::shared_ptr<Mesh>      &meshPtr) noexcept;
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
  void setLightPtr(const std::shared_ptr<BaseLight> &lightPtr) noexcept;
  void setMeshPtr(const std::shared_ptr<Mesh> &meshPtr) noexcept;

  // Getters
  const glm::vec3                  &getTranslate() const noexcept;
  glm::vec3                        &getTranslate() noexcept;
  const glm::vec3                  &getRotate() const noexcept;
  glm::vec3                        &getRotate() noexcept;
  const glm::vec3                  &getScale() const noexcept;
  glm::vec3                        &getScale() noexcept;
  const glm::mat4                  &getModelMatrix() const noexcept;
  glm::mat4                        &getModelMatrix() noexcept;
  const std::shared_ptr<BaseLight> &getLightPtr() const noexcept;
  std::shared_ptr<BaseLight>       &getLightPtr() noexcept;
  const std::shared_ptr<Mesh>      &getMeshPtr() const noexcept;
  std::shared_ptr<Mesh>            &getMeshPtr() noexcept;

  // Other member functions
  void recalculateModelMatrix() noexcept;

  void updateShaderModelMatrix() const noexcept;

  void render() const noexcept;

  // Other static member functions
  static void updateShadersCamera(const std::vector<SceneObject> &sceneObjects,
                                  const BaseCamera               &camera) noexcept;
  static void updateShadersLights(const std::vector<SceneObject> &sceneObjects) noexcept;
};

}  // namespace glengine

#endif
