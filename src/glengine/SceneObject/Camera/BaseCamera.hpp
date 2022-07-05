#ifndef GLENGINE_SCENEOBJECT_CAMERA_BASECAMERA_HPP
#define GLENGINE_SCENEOBJECT_CAMERA_BASECAMERA_HPP

// GLM
#include <glm/glm.hpp>

namespace glengine {

class BaseCamera {
 private:
  glm::vec3 _pos{};
  glm::vec3 _worldUp{};
  glm::vec3 _forward{};
  glm::vec3 _right{};
  glm::vec3 _up{};

 protected:
  // Constructors, assignment operators and destructor
  BaseCamera() noexcept;
  BaseCamera(const BaseCamera &camera) noexcept;
  BaseCamera &operator=(const BaseCamera &camera) noexcept;
  BaseCamera(BaseCamera &&camera) noexcept;
  BaseCamera &operator=(BaseCamera &&camera) noexcept;
  virtual ~BaseCamera() noexcept;

 public:
  // Setters
  void setPosition(const glm::vec3 &pos) noexcept;
  void setWorldUp(const glm::vec3 &worldUp) noexcept;

  // Getters
  const glm::vec3 &getPosition() const noexcept;
  glm::vec3       &getPosition() noexcept;
  const glm::vec3 &getWorldUpDirection() const noexcept;
  glm::vec3       &getWorldUpDirection() noexcept;
  const glm::vec3 &getForwardDirection() const noexcept;
  glm::vec3       &getForwardDirection() noexcept;
  const glm::vec3 &getRightDirection() const noexcept;
  glm::vec3       &getRightDirection() noexcept;
  const glm::vec3 &getUpDirection() const noexcept;
  glm::vec3       &getUpDirection() noexcept;

  // Other member functions
  void look(const glm::vec3 &look) noexcept;
  void lookAt(const glm::vec3 &lookAt) noexcept;

  glm::mat4         getViewMatrix() const noexcept;
  virtual glm::mat4 getProjectionMatrix() const noexcept = 0;

  friend class Camera3DoFController;
  friend class Camera5DoFController;
  friend class Camera6DoFController;
};

}  // namespace glengine

#endif
