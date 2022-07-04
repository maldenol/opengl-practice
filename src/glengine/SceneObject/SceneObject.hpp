#ifndef GLENGINE_SCENE_SCENE_HPP
#define GLENGINE_SCENE_SCENE_HPP

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

  // Unity-like components
  std::shared_ptr<BaseLight> _lightPtr{};
  std::shared_ptr<Mesh>      _meshPtr{};

 public:
  SceneObject() noexcept;
  SceneObject(const glm::vec3 &translate, const glm::vec3 &rotate, const glm::vec3 &scale,
              const std::shared_ptr<BaseLight> &lightPtr,
              const std::shared_ptr<Mesh>      &meshPtr) noexcept;
  SceneObject(const SceneObject &sceneObject) noexcept;
  SceneObject &operator=(const SceneObject &sceneObject) noexcept;
  SceneObject(SceneObject &&sceneObject) noexcept;
  SceneObject &operator=(SceneObject &&sceneObject) noexcept;
  ~SceneObject() noexcept;

  void setTranslate(const glm::vec3 &translate) noexcept;
  void setRotate(const glm::vec3 &rotate) noexcept;
  void setScale(const glm::vec3 &scale) noexcept;
  void setLightPtr(const std::shared_ptr<BaseLight> &lightPtr) noexcept;
  void setMeshPtr(const std::shared_ptr<Mesh> &meshPtr) noexcept;

  const glm::vec3                  &getTranslate() const noexcept;
  glm::vec3                        &getTranslate() noexcept;
  const glm::vec3                  &getRotate() const noexcept;
  glm::vec3                        &getRotate() noexcept;
  const glm::vec3                  &getScale() const noexcept;
  glm::vec3                        &getScale() noexcept;
  const std::shared_ptr<BaseLight> &getLightPtr() const noexcept;
  std::shared_ptr<BaseLight>       &getLightPtr() noexcept;
  const std::shared_ptr<Mesh>      &getMeshPtr() const noexcept;
  std::shared_ptr<Mesh>            &getMeshPtr() noexcept;

  void render(const BaseCamera &camera) const;
};

}  // namespace glengine

#endif
